// binary_operations.c
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../definitions.h"
#include "../error_handling/error.h"
#include "../global_variables/global_vars.h"
#include "../token_utils/token_utils.h"

#define OPERAND_A tokens[0]
#define OPERATOR tokens[1]
#define OPERAND_B tokens[2]

#define INT_TO_STR_SIZE 12
#define DOUBLE_TO_STR_SIZE 21

Token BinaryOperation(Token *tokens, size_t line_num)
{
  Token result_token;

  // Translate variables
  if (OPERAND_A.type == TOKEN_IDENTIFIER)
  {
    GetGlobalVariableResult variable = GetGlobalVariable(OPERAND_A.value);

    if (variable.variable_index != -1)
    {
      VariableType variable_type = TokenTypeToVariableType(OPERAND_A, line_num);
      if (variable_type == INVALID)
        error(line_num, SYNTAX_INVALID, "Cannot perform binary operation between operand type(s).");

      TokenType token_type = VariableTypeToTokenType(variable_type);
      if (token_type == TOKEN_NULL)
        error(line_num, TYPE_INVALID, "NULL_TOKEN from 'variable_type_to_token_type'.");

      OPERAND_A.type = token_type;
      OPERAND_A.value = (char *)variable.variable_value;
    }
    else
      error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier at binary operation.");
  }
  if (OPERAND_B.type == TOKEN_IDENTIFIER)
  {
    GetGlobalVariableResult variable = GetGlobalVariable(OPERAND_B.value);

    if (variable.variable_index != -1)
    {
      VariableType variable_type = TokenTypeToVariableType(OPERAND_B, line_num);
      if (variable_type == INVALID)
      {
        error(line_num, SYNTAX_INVALID, "Cannot perform binary operation between operand type(s).");
      }

      TokenType token_type = VariableTypeToTokenType(variable_type);
      if (token_type == TOKEN_NULL)
      {
        error(line_num, TYPE_INVALID, "NULL_TOKEN from 'variable_type_to_token_type'.");
      }

      OPERAND_B.type = token_type;
      OPERAND_B.value = (char *)variable.variable_value;
    }
    else
      error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier at binary operation.");
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

Token ParseBinaryOperation(Token *tokens, size_t tokens_count, size_t line_num)
{
  Token result;
  result.type = TOKEN_NULL;
  result.value = NULL;
  result.precedence = NO_PRECEDENCE;

  Token operation[3];

  // If there's no operation we return the input
  if (tokens_count == 2)
  {
    if (IS_VALID_BINARY_OPERAND(tokens[0].type))
    {
      if (tokens[0].type == TOKEN_IDENTIFIER)
      {
        GetGlobalVariableResult variable = GetGlobalVariable(tokens[0].value);

        if (variable.variable_index != -1)
        {
          result.type = VariableTypeToTokenType(variable.variable_type);
          result.value = variable.variable_value;
          result.precedence = NO_PRECEDENCE;
        }
        else
        {
          error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");
        }
      }
      else
      {
        result.type = tokens[0].type;
        result.value = tokens[0].value;
        result.precedence = tokens[0].precedence;
      }
    }
    else
    {
      error(line_num, SYNTAX_INVALID,
            "Incomplete binary operation or invalid input (binary_operations).");
    }
  }

  ssize_t i;
  for (i = 0; i < tokens_count; i++)
  {
    if (IS_BINARY_OPERATOR(CURRENT_TOKEN.type))
    {
      if (i - 1 > -1 && i + 1 <= tokens_count && IS_VALID_BINARY_OPERAND(PREVIOUS_TOKEN.type) &&
          IS_VALID_BINARY_OPERAND(NEXT_TOKEN_1.type))
      {
        // Next operation has more precedence.
        if (NEXT_TOKEN_2.precedence > CURRENT_TOKEN.precedence)
        {
          operation[0] = NEXT_TOKEN_1;
          operation[1] = NEXT_TOKEN_2;
          operation[2] = NEXT_TOKEN_3;

          result = BinaryOperation(operation, line_num);
          NEXT_TOKEN_1.type = result.type;
          NEXT_TOKEN_1.value = result.value;
          NEXT_TOKEN_1.precedence = result.precedence;

          // Shift tokens
          size_t shift_start = i + 2;
          size_t total_tokens_to_shift = 2;

          for (size_t j = shift_start; j + total_tokens_to_shift < tokens_count; j++)
          {
            tokens[j] = tokens[j + total_tokens_to_shift];
          }

          // Decrease the token count since we removed two
          tokens_count -= total_tokens_to_shift;

          i -= total_tokens_to_shift;
        }

        // Next operation has no precedence.
        else
        {
          operation[0] = PREVIOUS_TOKEN;
          operation[1] = CURRENT_TOKEN;
          operation[2] = NEXT_TOKEN_1;

          result = BinaryOperation(operation, line_num);
          PREVIOUS_TOKEN.type = result.type;
          PREVIOUS_TOKEN.value = result.value;
          PREVIOUS_TOKEN.precedence = result.precedence;

          // Shift tokens
          size_t shift_start = i;
          size_t total_tokens_to_shift = 2;

          for (size_t j = shift_start; j + total_tokens_to_shift < tokens_count; j++)
          {
            tokens[j] = tokens[j + total_tokens_to_shift];
          }

          // Decrease the token count since we removed two
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