// binary_operations.c
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/core/interpreter/interpreter.h"
#include "csarian/definitions.h"
#include "csarian/main.h"
#include "csarian/runtime/functions/fn.h"
#include "csarian/runtime/variables/global_variables/global_vars.h"
#include "csarian/runtime/variables/local_variables/local_vars.h"
#include "csarian/utils/debug/debug.h"
#include "csarian/utils/token_utils/token_utils.h"

static void ReturnInput(Token *tokens, ssize_t current_function, Token *result, size_t line_num)
{
  if (IS_VALID_BINARY_OPERAND(tokens[0].type))
  {
    if (tokens[0].type == TOKEN_IDENTIFIER)
    {
      GetGlobalVariableResult global_variable = GetGlobalVariable(tokens[0].value);

      GetLocalVariableResult local_variable;
      local_variable.variable_index = -1;  // Makes sure we don't read garbage.
      if (current_function != -1)
      {
        local_variable = GetLocalVariable(current_function, tokens[0].value);
      }

      if (current_function != -1)
      {
        if (local_variable.variable_index != -1)
        {
          result->type = VariableTypeToTokenType(local_variable.variable_type);
          result->value = local_variable.variable_value;
          result->precedence = NO_PRECEDENCE;
        }
        else if (global_variable.variable_index != -1)
        {
          result->type = VariableTypeToTokenType(global_variable.variable_type);
          result->value = global_variable.variable_value;
          result->precedence = NO_PRECEDENCE;
        }
        else
        {
          error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");
        }
      }
      else
      {
        if (global_variable.variable_index != -1)
        {
          result->type = VariableTypeToTokenType(global_variable.variable_type);
          result->value = global_variable.variable_value;
          result->precedence = NO_PRECEDENCE;
        }
        else
        {
          error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");
        }
      }
    }
    else
    {
      result->type = tokens[0].type;
      result->value = tokens[0].value;
      result->precedence = tokens[0].precedence;
    }
  }
  else
  {
    error(line_num, SYNTAX_INCOMPLETE_EXPRESSION, "Incomplete binary operation.");
  }
}

static Token TranslateVariable(Token token, ssize_t current_function, size_t line_num)
{
  GetGlobalVariableResult global_variable = GetGlobalVariable(token.value);

  GetLocalVariableResult local_variable;
  local_variable.variable_index = -1;
  if (current_function != -1)
  {
    local_variable = GetLocalVariable(current_function, token.value);
  }

  if (current_function != -1)
  {
    if (local_variable.variable_index != -1)
    {
      VariableType variable_type = TokenTypeToVariableType(token, current_function, line_num);
      if (variable_type == INVALID)
        error(line_num, SYNTAX_INVALID, "Cannot perform binary operation between operand type(s).");

      TokenType token_type = VariableTypeToTokenType(variable_type);
      if (token_type == TOKEN_NULL)
        error(line_num, TYPE_INVALID, "NULL_TOKEN from 'variable_type_to_token_type'.");

      token.type = token_type;
      token.value = (char *)local_variable.variable_value;
    }

    else if (global_variable.variable_index != -1)
    {
      VariableType variable_type = TokenTypeToVariableType(token, current_function, line_num);
      if (variable_type == INVALID)
        error(line_num, SYNTAX_INVALID, "Cannot perform binary operation between operand type(s).");

      TokenType token_type = VariableTypeToTokenType(variable_type);
      if (token_type == TOKEN_NULL)
        error(line_num, TYPE_INVALID, "NULL_TOKEN from 'variable_type_to_token_type'.");

      token.type = token_type;
      token.value = (char *)global_variable.variable_value;
    }

    else
      error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");
  }

  else
  {
    if (global_variable.variable_index != -1)
    {
      VariableType variable_type = TokenTypeToVariableType(token, current_function, line_num);
      if (variable_type == INVALID)
        error(line_num, SYNTAX_INVALID, "Cannot perform binary operation between operand type(s).");

      TokenType token_type = VariableTypeToTokenType(variable_type);
      if (token_type == TOKEN_NULL)
        error(line_num, TYPE_INVALID, "NULL_TOKEN from 'variable_type_to_token_type'.");

      token.type = token_type;
      token.value = (char *)global_variable.variable_value;
    }
    else
      error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");
  }

  return token;
}

#define OPERAND_A tokens[0]
#define OPERATOR tokens[1]
#define OPERAND_B tokens[2]

static Token BinaryOperation(Token *tokens, ssize_t current_function, size_t line_num)
{
  Token result_token;

  // Translate variables
  if (OPERAND_A.type == TOKEN_IDENTIFIER)
  {
    Token result = TranslateVariable(OPERAND_A, current_function, line_num);
    OPERAND_A.type = result.type;
    OPERAND_A.value = result.value;
  }
  if (OPERAND_B.type == TOKEN_IDENTIFIER)
  {
    Token result = TranslateVariable(OPERAND_B, current_function, line_num);
    OPERAND_B.type = result.type;
    OPERAND_B.value = result.value;
  }

  // String operations
  if (OPERAND_A.type == TOKEN_STRING || OPERAND_B.type == TOKEN_STRING)
  {
    if (OPERAND_A.type == TOKEN_STRING && OPERAND_B.type == TOKEN_STRING ||
        OPERAND_B.type == TOKEN_INT_LITERAL)
    {
      if (OPERATOR.type == TOKEN_PLUS)
      {
        if (OPERAND_A.type == TOKEN_STRING && OPERAND_B.type == TOKEN_STRING)
        {
          char *string1 = OPERAND_A.value;
          char *string2 = OPERAND_B.value;

          char *result = malloc(strlen(string1) + strlen(string2) + 1);
          if (!result)
            error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");

          result[0] = '\0';

          strcat(result, string1);
          strcat(result, string2);

          result_token.type = TOKEN_STRING;
          result_token.value = result;
          result_token.precedence = NO_PRECEDENCE;

          return result_token;
        }
        else
        {
          error(line_num, TYPE_INVALID, "Can only concatenate string to string.");
        }
      }

      else if (OPERATOR.type == TOKEN_ASTERISK)
      {
        if (OPERAND_B.type == TOKEN_INT_LITERAL)
        {
          unsigned int multiply_times = atoi(OPERAND_B.value);

          char *string1 = malloc(strlen(OPERAND_A.value) * multiply_times + 1);
          if (!string1)
            error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() string1.");

          string1[0] = '\0';

          char *string1_tmp = OPERAND_A.value;

          for (size_t i = 0; i < multiply_times; i++)
          {
            strcat(string1, string1_tmp);
          }

          result_token.type = TOKEN_STRING;
          result_token.value = string1;
          result_token.precedence = NO_PRECEDENCE;

          return result_token;
        }
        else
        {
          error(line_num, TYPE_INVALID, "Expected integer for string multiplication.");
        }
      }
      else
      {
        error(line_num, TYPE_INVALID, "Can only use '+' and '*' for string tokens.");
      }
    }
    else
    {
      error(line_num, TYPE_INVALID, "Operator not allowed between operand type(s).");
    }
  }

  // Numeric operations
  else
  {
    bool is_float;

    int int_result;
    double float_result;

    if (OPERAND_A.type == TOKEN_FLOAT_LITERAL || OPERAND_B.type == TOKEN_FLOAT_LITERAL)
    {
      is_float = true;
    }
    else
    {
      is_float = false;
    }

    switch (OPERATOR.type)
    {
      case TOKEN_PLUS:
        if (is_float == true)
        {
          float_result = atof(OPERAND_A.value) + atof(OPERAND_B.value);
        }
        else
        {
          int_result = atoi(OPERAND_A.value) + atoi(OPERAND_B.value);
        }
        break;
      case TOKEN_MINUS:
        if (is_float == true)
        {
          float_result = atof(OPERAND_A.value) - atof(OPERAND_B.value);
        }
        else
        {
          int_result = atoi(OPERAND_A.value) - atoi(OPERAND_B.value);
        }
        break;
      case TOKEN_ASTERISK:
        if (is_float == true)
        {
          float_result = atof(OPERAND_A.value) * atof(OPERAND_B.value);
        }
        else
        {
          int_result = atoi(OPERAND_A.value) * atoi(OPERAND_B.value);
        }
        break;
      case TOKEN_SLASH:
        is_float = true;
        float_result = atof(OPERAND_A.value) / atof(OPERAND_B.value);
        break;
      case TOKEN_PERCENT:
        if (is_float == true)
        {
          float_result = fmod(atof(OPERAND_A.value), atof(OPERAND_B.value));
        }
        else
        {
          int_result = atoi(OPERAND_A.value) % atoi(OPERAND_B.value);
        }
        break;

      default:
        error(line_num, TOKENERR_UNKNOWN_TOKEN, "Unknown operator type.");
    }

    if (!is_float)
    {
      result_token.type = TOKEN_INT_LITERAL;
      char buffer[INT_TO_STR_SIZE];
      sprintf(buffer, "%d", int_result);
      result_token.value = strdup(buffer);
      result_token.precedence = NO_PRECEDENCE;
    }
    else
    {
      result_token.type = TOKEN_FLOAT_LITERAL;
      char buffer[DOUBLE_TO_STR_SIZE];
      sprintf(buffer, "%lf", float_result);
      result_token.value = strdup(buffer);
      result_token.precedence = NO_PRECEDENCE;
    }
  }

  return result_token;
}

Token ParseBinaryOperation(Token *tokens, size_t tokens_count, ssize_t current_function,
                           size_t line_num)
{
  Token result;
  result.type = TOKEN_NULL;
  result.value = NULL;
  result.precedence = NO_PRECEDENCE;

  Token operation[3];

  // Translate function calls
  for (size_t i = 0; i < tokens_count; i++)
  {
    if (I_CURRENT_TOKEN.type == TOKEN_IDENTIFIER && I_NEXT_TOKEN_1.type == TOKEN_LPARENT)
    {
      ssize_t index_result = SearchFunction(I_CURRENT_TOKEN.value);

      if (index_result == -1)
      {
        error(line_num, IDENTIFIER_UNKNOWN, "Unknown function at binary operation.");
      }

      // Arguments
      ResultTokens parent_tokens = *GetParentTokens(&I_NEXT_TOKEN_1, tokens_count, line_num);

      ResultVariables *arguments = GetFunctionArguments(
        parent_tokens.result_tokens, parent_tokens.result_tokens_count, current_function, line_num);

      if (arguments->result_variables_count != functions[index_result].arguments)
      {
        if (arguments->result_variables_count > functions[index_result].arguments)
          error(line_num, TYPE_INVALID_ARGUMENTS, "Expected less arguments for function call.");

        if (arguments->result_variables_count < functions[index_result].arguments)
          error(line_num, TYPE_INVALID_ARGUMENTS, "Expected more arguments for function call.");
      }

      for (size_t j = 0; j < functions[index_result].arguments; j++)
      {
        functions[index_result].function_variables[j].type = arguments->result_variables[j].type;
        functions[index_result].function_variables[j].value = arguments->result_variables[j].value;
      }

      Token function_return =
        Interpreter(main_tokens.result_tokens, main_tokens.result_tokens_count, true, index_result,
                    line_num, functions[index_result].end, main_tokens.result_tokens_count - 1,
                    functions[index_result].start + 1, false);

      // Shift tokens
      if (function_return.value == NULL)
      {
        error(line_num, TYPE_INVALID, "Function return is null.");
      }

      I_CURRENT_TOKEN.type = function_return.type;
      I_CURRENT_TOKEN.value = function_return.value;
      I_CURRENT_TOKEN.precedence = function_return.precedence;

      size_t shift_start = i + 1;
      size_t total_tokens_to_shift = parent_tokens.result_tokens_count + 1;

      for (size_t j = shift_start; j + total_tokens_to_shift < tokens_count; j++)
      {
        tokens[j] = tokens[j + total_tokens_to_shift];
      }

      tokens_count -= total_tokens_to_shift;
    }
  }

  // If there's no operation we return the input
  if (tokens_count == 2)
  {
    ReturnInput(tokens, current_function, &result, line_num);
    return result;
  }

  for (size_t i = 0; i < tokens_count; i++)
  {
    if (IS_BINARY_OPERATOR(I_CURRENT_TOKEN.type))
    {
      if (i - 1 >= 0 && i + 1 <= tokens_count && IS_VALID_BINARY_OPERAND(I_PREVIOUS_TOKEN.type) &&
          IS_VALID_BINARY_OPERAND(I_NEXT_TOKEN_1.type))
      {
        // Next operation has more precedence.
        if (I_NEXT_TOKEN_2.precedence > I_CURRENT_TOKEN.precedence)
        {
          operation[0] = I_NEXT_TOKEN_1;
          operation[1] = I_NEXT_TOKEN_2;
          operation[2] = I_NEXT_TOKEN_3;

          result = BinaryOperation(operation, current_function, line_num);
          I_NEXT_TOKEN_1.type = result.type;
          I_NEXT_TOKEN_1.value = result.value;
          I_NEXT_TOKEN_1.precedence = result.precedence;

          // Shift tokens
          size_t shift_start = i + 2;
          size_t total_tokens_to_shift = 2;

          for (size_t j = shift_start; j + total_tokens_to_shift < tokens_count; j++)
          {
            tokens[j] = tokens[j + total_tokens_to_shift];
          }

          tokens_count -= total_tokens_to_shift;

          i -= total_tokens_to_shift;
        }

        // Next operation has no precedence.
        else
        {
          operation[0] = I_PREVIOUS_TOKEN;
          operation[1] = I_CURRENT_TOKEN;
          operation[2] = I_NEXT_TOKEN_1;

          result = BinaryOperation(operation, current_function, line_num);
          I_PREVIOUS_TOKEN.type = result.type;
          I_PREVIOUS_TOKEN.value = result.value;
          I_PREVIOUS_TOKEN.precedence = result.precedence;

          // Shift tokens
          size_t shift_start = i;
          size_t total_tokens_to_shift = 2;

          for (size_t j = shift_start; j + total_tokens_to_shift < tokens_count; j++)
          {
            tokens[j] = tokens[j + total_tokens_to_shift];
          }

          tokens_count -= total_tokens_to_shift;

          i -= total_tokens_to_shift;
        }
      }
      else
      {
        error(line_num, TYPE_INVALID_OPERANDS, "Invalid operation operands.");
      }
    }
  }

  return result;
}