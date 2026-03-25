#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "definitions.h"
#include "binary_operations/binary_operations.h"
#include "error_handling/error.h"
#include "binary_operations/binary_operations.h"
#include "global_vars/global_vars.h"

#define PREVIOUS_TOKEN tokens[i - 1]
#define CURRENT_TOKEN tokens[i]
#define NEXT_TOKEN_1 tokens[i + 1]
#define NEXT_TOKEN_2 tokens[i + 2]
#define NEXT_TOKEN_3 tokens[i + 3]

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