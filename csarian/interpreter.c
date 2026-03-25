#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "definitions.h"
#include "error.h"

#define PREVIOUS_TOKEN tokens[i - 1]
#define CURRENT_TOKEN tokens[i]
#define NEXT_TOKEN_1 tokens[i + 1]
#define NEXT_TOKEN_2 tokens[i + 2]
#define NEXT_TOKEN_3 tokens[i + 3]
#define INT_TO_STR_SIZE 12
#define DOUBLE_TO_STR_SIZE 21

// For the binary operation function
#define OPERAND_A tokens[0]
#define OPERATOR tokens[1]
#define OPERAND_B tokens[2]

// Global Variables
GlobalVariable *global_variables;
size_t global_variables_count;
size_t global_variables_size;

void InitGlobalVariables()
{
  global_variables = calloc(32, sizeof(GlobalVariable));
  global_variables_size = 32;
  global_variables_count = 0;
}

void TerminateGlobalVariables()
{
  free(global_variables);
  global_variables = NULL;
}

void CreateGlobalVariable(char *name, VariableType type, void *value)
{
  if (global_variables_count > global_variables_size)
  {
    size_t new_size = global_variables_size * 2;

    GlobalVariable *tmp = realloc(global_variables, sizeof(GlobalVariable) * new_size);
    global_variables = tmp;

    global_variables_size *= 2;
  }

  global_variables[global_variables_count].key = name;
  global_variables[global_variables_count].type = type;
  global_variables[global_variables_count].value = value;
  global_variables_count++;
}

typedef struct
{
  int variable_index;
  void *variable_value;
  VariableType variable_type;
} GetGlobalVariableResult;

GetGlobalVariableResult GetGlobalVariable(char *name)
{
  GetGlobalVariableResult result;

  for (size_t i = 0; i < global_variables_count; i++)
  {
    if (strcmp(global_variables[i].key, name) == 0)
    {
      result.variable_index = i;
      result.variable_type = global_variables[i].type;
      result.variable_value = global_variables[i].value;
      return result;
    }
  }

  // Variable is not found
  result.variable_index = -1;
  result.variable_value = NULL;
  return result;
}

// Other functions
VariableType token_type_to_variable_type(Token token, size_t line_num)
{
  VariableType result;
  GetGlobalVariableResult variable_result;

  switch (token.type)
  {
    case TOKEN_INT_LITERAL:
      result = INT;
      break;
    case TOKEN_FLOAT_LITERAL:
      result = FLOAT;
      break;
    case TOKEN_STRING:
      result = STRING;
      break;
    case TOKEN_IDENTIFIER:
      variable_result = GetGlobalVariable(token.value);
      if (variable_result.variable_index == -1) {error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");}

      result = variable_result.variable_type;
      break;
    
    default: 
      result = INVALID;
      break;
  }

  return result;
}

TokenType variable_type_to_token_type(VariableType type)
{
  TokenType result;

  switch (type)
  {
    case INT:
      result = TOKEN_INT_LITERAL;
      break;
    case FLOAT:
      result = TOKEN_FLOAT_LITERAL;
      break;
    case STRING:
      result = TOKEN_STRING;
      break;
    
    default: 
      result = TOKEN_NULL;
      break;
  }

  return result;
}

typedef struct
{
  size_t result_tokens_count;
  Token *result_tokens;
} ResultTokens;

ResultTokens *GetParentTokens(Token *tokens, size_t tokens_count, size_t line_num)
{
  ResultTokens *result = malloc(sizeof(ResultTokens));
  if (!result) {error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");}

  result->result_tokens_count = 0;
  size_t result_tokens_size = 1;

  result->result_tokens = calloc(result_tokens_size, sizeof(Token));
  if (!result->result_tokens) {error(line_num, MEM_CALLOC_FAILED, "Failed to calloc() result_tokens.");}

  for (size_t i = 0; i < tokens_count; i++)
  {
    if (CURRENT_TOKEN.type == TOKEN_EOF)
    {
      result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
      result->result_tokens[result->result_tokens_count].value = NULL;
      result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
      result->result_tokens_count++;

      return result;
    }

    else if (CURRENT_TOKEN.type == TOKEN_LPARENT)
    {
      for (size_t j = i + 1; j < tokens_count; j++)
      {
        if (tokens[j].type == TOKEN_RPARENT)
        {
          result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
          result->result_tokens[result->result_tokens_count].value = NULL;
          result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
          result->result_tokens_count++;

          return result;
        }

        else if (tokens[j].type == TOKEN_EOF) {error(line_num, SYNTAX_INCOMPLETE_PARENT, "Incomplete parents.");}

        else 
        {
          result->result_tokens[result->result_tokens_count].type = tokens[j].type;
          result->result_tokens[result->result_tokens_count].value = tokens[j].value;
          result->result_tokens[result->result_tokens_count].precedence = tokens[j].precedence;
          result->result_tokens_count++;

          Token *tmp = realloc(result->result_tokens, (result_tokens_size + 1) * sizeof(Token));
          if (!tmp) {error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");}

          result_tokens_size++;
          result->result_tokens = tmp;
        }
      }
    }
  }

  result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
  result->result_tokens[result->result_tokens_count].value = NULL;
  result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
  result->result_tokens_count++;

  return result;
}

Token BinaryOperation(Token *tokens, size_t line_num)
{
  Token result_token;

  // Translate variables
  if (OPERAND_A.type == TOKEN_IDENTIFIER)
  {
    GetGlobalVariableResult variable = GetGlobalVariable(OPERAND_A.value);

    if (variable.variable_index != -1)
    {
      VariableType variable_type = token_type_to_variable_type(OPERAND_A, line_num);
      if (variable_type == INVALID) {error(line_num, SYNTAX_INVALID, "Cannot perform binary operation between operand type(s).");}

      TokenType token_type = variable_type_to_token_type(variable_type);
      if (token_type == TOKEN_NULL) {error(line_num, TYPE_INVALID, "NULL_TOKEN from 'variable_type_to_token_type'.");}

      OPERAND_A.type = token_type;
      OPERAND_A.value = (char*)variable.variable_value;
    }
    else {error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier at binary operation.");}
  }
  if (OPERAND_B.type == TOKEN_IDENTIFIER)
  {
    GetGlobalVariableResult variable = GetGlobalVariable(OPERAND_B.value);

    if (variable.variable_index != -1)
    {
      VariableType variable_type = token_type_to_variable_type(OPERAND_B, line_num);
      if (variable_type == INVALID) {error(line_num, SYNTAX_INVALID, "Cannot perform binary operation between operand type(s).");}

      TokenType token_type = variable_type_to_token_type(variable_type);
      if (token_type == TOKEN_NULL) {error(line_num, TYPE_INVALID, "NULL_TOKEN from 'variable_type_to_token_type'.");}

      OPERAND_B.type = token_type;
      OPERAND_B.value = (char*)variable.variable_value;
    }
    else {error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier at binary operation.");}
  }

  // Strings
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
          if (!result) {error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");}

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
          if (!string1) {error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() string1.");}

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

  // Numeric
  else 
  {
    bool is_float;

    int int_result;
    double float_result;

    if (OPERAND_A.type == TOKEN_FLOAT_LITERAL || OPERAND_B.type == TOKEN_FLOAT_LITERAL)
    {
      is_float = true;
    }
    else {is_float = false;}

    switch (OPERATOR.type) 
    {
      case TOKEN_PLUS:
        if (is_float == true) {float_result = atof(OPERAND_A.value) + atof(OPERAND_B.value);}
        else {int_result = atoi(OPERAND_A.value) + atoi(OPERAND_B.value);}
        break;
      case TOKEN_MINUS:
        if (is_float == true) {float_result = atof(OPERAND_A.value) - atof(OPERAND_B.value);}
        else {int_result = atoi(OPERAND_A.value) - atoi(OPERAND_B.value);}
        break;
      case TOKEN_ASTERISK:
        if (is_float == true) {float_result = atof(OPERAND_A.value) * atof(OPERAND_B.value);}
        else {int_result = atoi(OPERAND_A.value) * atoi(OPERAND_B.value);}
        break;
      case TOKEN_SLASH:
        is_float = true;
        float_result = atof(OPERAND_A.value) / atof(OPERAND_B.value);
        break;
      case TOKEN_PERCENT:
        if (is_float == true) {float_result = fmod(atof(OPERAND_A.value), atof(OPERAND_B.value));}
        else {int_result = atoi(OPERAND_A.value) % atoi(OPERAND_B.value);}
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
  result.type = TOKEN_NULL; result.value = NULL; result.precedence = NO_PRECEDENCE;

  Token operation[3];

  size_t i;
  for (i = 0; i < tokens_count; i++)
  {
    if (IS_BINARY_OPERATOR(CURRENT_TOKEN.type))
    {
      if (IS_VALID_BINARY_OPERAND(PREVIOUS_TOKEN.type) && IS_VALID_BINARY_OPERAND(NEXT_TOKEN_1.type))
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

          for (size_t j = shift_start; j + total_tokens_to_shift < tokens_count; j++) {
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

          // Make the first operand the result and remove the operator 
          // and second operand so we can continue with the operation
          PREVIOUS_TOKEN.type = result.type;
          PREVIOUS_TOKEN.value = result.value;
          PREVIOUS_TOKEN.precedence = result.precedence;

          size_t shift_start = i;
          size_t total_tokens_to_shift = 2;

          for (size_t j = shift_start; j + total_tokens_to_shift < tokens_count; j++) {
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

bool Comparison();

int Interpreter(Token *tokens, size_t tokens_count)
{
  InitGlobalVariables();
  
  size_t line_num = 1;

  size_t i;
  for (i = 0; i < tokens_count; i++)
  {
    if (CURRENT_TOKEN.type == TOKEN_EOL)
    {
      line_num++;
    }

    if (CURRENT_TOKEN.type == TOKEN_DBG_PRINT)
    {
      ResultTokens print_tokens = *GetParentTokens(&tokens[i], tokens_count, line_num);

      Token result_token = ParseBinaryOperation(print_tokens.result_tokens, print_tokens.result_tokens_count, line_num);

      if (result_token.type != TOKEN_NULL)
      {
        printf("%s\n",result_token.value);
      }
      else
      {
        if (print_tokens.result_tokens->value)
        {
          if (print_tokens.result_tokens->type == TOKEN_IDENTIFIER)
          {
            GetGlobalVariableResult variable = GetGlobalVariable(print_tokens.result_tokens->value);

            if (variable.variable_index != -1)
            {
              printf("%s\n",(char*)variable.variable_value);
            }
            else {fprintf(stderr, "Variable not found.\n");}
          }
          else {printf("%s\n",print_tokens.result_tokens->value);}
        }
        else
        {
          printf("\n");
        }
      }
    }

    if (CURRENT_TOKEN.type == TOKEN_ASSIGNMENT)
    {
      if (i - 1 > 0 && i + 1 < tokens_count) // Make sure we don't underflow/overflow
      {
        int variable_index;
        if (PREVIOUS_TOKEN.type == TOKEN_IDENTIFIER)
        {
          variable_index = GetGlobalVariable(PREVIOUS_TOKEN.value).variable_index;
        }
        else {error(line_num, SYNTAX_INVALID, "Variable name is not an identifier.");}

        // Variable exists
        if (variable_index != -1)
        {
          VariableType variable_type = token_type_to_variable_type(NEXT_TOKEN_1, line_num);

          if (variable_type != INVALID)
          {
            global_variables[variable_index].type = variable_type;
            global_variables[variable_index].value = NEXT_TOKEN_1.value;
          }
          else {error(line_num, TYPE_INVALID, "Invalid variable value.");}
        }
        // Variable doesn't exist
        else
        {
          VariableType variable_type = token_type_to_variable_type(NEXT_TOKEN_1, line_num);

          if (variable_type != INVALID)
          {
            if (NEXT_TOKEN_1.type == TOKEN_IDENTIFIER)
            {
              GetGlobalVariableResult variable_result = GetGlobalVariable(NEXT_TOKEN_1.value);
              if (variable_result.variable_index == -1) {error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier at variable assignment");}
              
              CreateGlobalVariable(PREVIOUS_TOKEN.value, variable_type, variable_result.variable_value);
            }
            else 
            {
              CreateGlobalVariable(PREVIOUS_TOKEN.value, variable_type, NEXT_TOKEN_1.value);
            }
          }
          else {error(line_num, TYPE_INVALID, "Invalid variable value.");}
        }
      }
      else {error(line_num, SYNTAX_INCOMPLETE_ASSIGNMENT, "Incomplete assignment (=).");}
    }
  }

  TerminateGlobalVariables();
  return 0;
}