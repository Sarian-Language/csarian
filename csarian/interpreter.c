// interpreter.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "debug/debug.h"
#include "definitions.h"
#include "binary_operations/binary_operations.h"
#include "error_handling/error.h"
#include "binary_operations/binary_operations.h"
#include "global_vars/global_vars.h"
#include "functions/fn.h"

#define PREVIOUS_TOKEN tokens[i - 1]
#define CURRENT_TOKEN tokens[i]
#define NEXT_TOKEN_1 tokens[i + 1]
#define NEXT_TOKEN_2 tokens[i + 2]
#define NEXT_TOKEN_3 tokens[i + 3]

#define J_PREVIOUS_TOKEN tokens[j - 1]
#define J_CURRENT_TOKEN tokens[j]
#define J_NEXT_TOKEN_1 tokens[j + 1]
#define J_NEXT_TOKEN_2 tokens[j + 2]
#define J_NEXT_TOKEN_3 tokens[j + 3]

VariableType TokenTypeToVariableType(Token token, size_t line_num)
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
      if (variable_result.variable_index == -1)
        error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");

      result = variable_result.variable_type;
      break;
    
    default: 
      result = INVALID;
      break;
  }

  return result;
}

TokenType VariableTypeToTokenType(VariableType type)
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
  if (!result) error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");

  result->result_tokens_count = 0;
  size_t result_tokens_size = 16;

  result->result_tokens = calloc(result_tokens_size, sizeof(Token));
  if (!result->result_tokens) error(line_num, MEM_CALLOC_FAILED, "Failed to calloc() result_tokens.");

  for (size_t i = 0; i < tokens_count; i++)
  {
    if (CURRENT_TOKEN.type == TOKEN_EOF)
    {
      if (result->result_tokens_count > result_tokens_size)
      {
        size_t new_size = result_tokens_size + 1;

        Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
        if (!tmp) error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

        result_tokens_size++;
        result->result_tokens = tmp;
      }

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
        if (J_CURRENT_TOKEN.type == TOKEN_LPARENT)
          error(line_num, SYNTAX_INCOMPLETE_PARENT, "Expected ')'.");

        else if (J_CURRENT_TOKEN.type == TOKEN_RPARENT)
        {
          if (result->result_tokens_count >= result_tokens_size)
          {
            size_t new_size = result_tokens_size + 1;

            Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
            if (!tmp) error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

            result_tokens_size++;
            result->result_tokens = tmp;
          }

          result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
          result->result_tokens[result->result_tokens_count].value = NULL;
          result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
          result->result_tokens_count++;

          return result;
        }

        else if (J_CURRENT_TOKEN.type == TOKEN_EOF) 
          error(line_num, SYNTAX_INCOMPLETE_PARENT, "Incomplete parents.");

        else 
        {
          if (result->result_tokens_count >= result_tokens_size)
          {
            size_t new_size = result_tokens_size * 2;

            Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
            if (!tmp) error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

            result_tokens_size *= 2;
            result->result_tokens = tmp;
          }

          result->result_tokens[result->result_tokens_count].type = J_CURRENT_TOKEN.type;
          result->result_tokens[result->result_tokens_count].value = J_CURRENT_TOKEN.value;
          result->result_tokens[result->result_tokens_count].precedence = J_CURRENT_TOKEN.precedence;
          result->result_tokens_count++;
        }
      }
    }
  }

  if (result->result_tokens_count >= result_tokens_size)
  {
    size_t new_size = result_tokens_size + 1;

    Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
    if (!tmp) error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

    result_tokens_size++;
    result->result_tokens = tmp;
  }

  result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
  result->result_tokens[result->result_tokens_count].value = NULL;
  result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
  result->result_tokens_count++;

  return result;
}

ResultTokens *GetTokensUntilEOL(Token *tokens, size_t tokens_count, size_t line_num)
{
  ResultTokens *result = malloc(sizeof(ResultTokens));
  if (!result) error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");

  result->result_tokens_count = 0;
  size_t result_tokens_size = 16;

  result->result_tokens = calloc(result_tokens_size, sizeof(Token));
  if (!result->result_tokens) error(line_num, MEM_CALLOC_FAILED, "Failed to calloc() result_tokens.");

  for (size_t i = 0; i < tokens_count; i++)
  {
    if (CURRENT_TOKEN.type == TOKEN_EOF)
    {
      if (result->result_tokens_count >= result_tokens_size)
      {
        size_t new_size = result_tokens_size + 1;

        Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
        if (!tmp) error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

        result_tokens_size++;
        result->result_tokens = tmp;
      }

      result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
      result->result_tokens[result->result_tokens_count].value = NULL;
      result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
      result->result_tokens_count++;

      return result;
    }

    else if (CURRENT_TOKEN.type == TOKEN_EOL)
    {
      if (result->result_tokens_count >= result_tokens_size)
      {
        size_t new_size = result_tokens_size + 1;

        Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
        if (!tmp) error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

        result_tokens_size++;
        result->result_tokens = tmp;
      }

      result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
      result->result_tokens[result->result_tokens_count].value = NULL;
      result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
      result->result_tokens_count++;

      return result;
    }

    else 
    {
      if (result->result_tokens_count >= result_tokens_size)
      {
        size_t new_size = result_tokens_size * 2;

        Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
        if (!tmp) error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

        result_tokens_size *= 2;
        result->result_tokens = tmp;
      }

      result->result_tokens[result->result_tokens_count].type = CURRENT_TOKEN.type;
      result->result_tokens[result->result_tokens_count].value = CURRENT_TOKEN.value;
      result->result_tokens[result->result_tokens_count].precedence = CURRENT_TOKEN.precedence;
      result->result_tokens_count++;
    }
  }

  result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
  result->result_tokens[result->result_tokens_count].value = NULL;
  result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
  result->result_tokens_count++;

  return result;
}

int Interpreter(Token *tokens, size_t tokens_count)
{
  InitGlobalVariables();
  InitFunctions();
  
  size_t line_num = 1;

  // For functions
  bool in_function = false;
  ssize_t block_end = -1;
  ssize_t original_pos = -1;

  size_t i;
  for (i = 0; i < tokens_count; i++)
  {
    if (CURRENT_TOKEN.type == TOKEN_EOL)
    {
      line_num++;
    }

    if (in_function == true)
    {
      if (i > block_end) // Check if we reached the end of the function
      {
        i = original_pos;

        original_pos = -1;
        in_function = false;
        block_end = -1;

        continue;
      }
    }

    if (CURRENT_TOKEN.type == TOKEN_FN)
    {
      if (i + 1 <= tokens_count && NEXT_TOKEN_1.type == TOKEN_IDENTIFIER)
      {
        if (i + 2 <= tokens_count && NEXT_TOKEN_2.type == TOKEN_LPARENT)
        {
          ResultTokens *parent_tokens = GetParentTokens(&NEXT_TOKEN_2, tokens_count - (i + 2), line_num);

          size_t fn_block_start = 0;
          size_t fn_block_end = 0;

          for (size_t j = i + 2 + parent_tokens->result_tokens_count + 1; j < tokens_count; j++)
          {
            if (J_CURRENT_TOKEN.type == TOKEN_LBRACKET)
            {
              if (fn_block_start == 0)
              {
                fn_block_start = j + 1;
              }
              else 
                error(line_num, SYNTAX_INVALID, "Nested brackets are not supported.");
            }

            if (J_CURRENT_TOKEN.type == TOKEN_RBRACKET)
            {
              if (fn_block_end == 0)
              {
                fn_block_end = j - 1;
                break;
              }
            }
          }

          AddFunction(NEXT_TOKEN_1.value, fn_block_start, fn_block_end);

          i = fn_block_end + 1;
          continue;
        }
        else 
          error(line_num, SYNTAX_INVALID, "Expected '('.");
      }
      else 
        error(line_num, SYNTAX_INVALID, "Expected function name after 'fn'.");
    }

    if (CURRENT_TOKEN.type == TOKEN_IDENTIFIER)
    {
      // Search for functions
      int result = SearchFunction(CURRENT_TOKEN.value);

      if (result != -1)
      {
        // Search for the function call end so we can
        // jump after it after being done with the function.
        for (size_t j = i; j < tokens_count; j++)
        {
          if (J_CURRENT_TOKEN.type == TOKEN_RPARENT)
          {
            original_pos = j;
            break;
          }
        }

        i = functions[result].start - 1;
        block_end = functions[result].end;
        in_function = true;

        continue;
      }
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
            else
              fprintf(stderr, "Variable not found.\n");
          }
          else
            printf("%s\n",print_tokens.result_tokens->value);
        }
        else
          printf("\n");
      }
    }

    if (CURRENT_TOKEN.type == TOKEN_ASSIGNMENT)
    {
      if (i - 1 >= 0 && i + 1 < tokens_count) // Make sure we don't underflow/overflow
      {
        int variable_index;

        if (PREVIOUS_TOKEN.type == TOKEN_IDENTIFIER)
        {
          variable_index = GetGlobalVariable(PREVIOUS_TOKEN.value).variable_index;
        }
        else
          error(line_num, SYNTAX_INVALID, "Expected identifier before '='.");

        // Variable exists
        if (variable_index != -1)
        {
          // Parse binary operation if there's one.
          ResultTokens result_tokens = *GetTokensUntilEOL(&NEXT_TOKEN_1, tokens_count - (i + 1), line_num);

          Token binary_operation_result = ParseBinaryOperation(result_tokens.result_tokens, result_tokens.result_tokens_count, line_num);

          VariableType variable_type = TokenTypeToVariableType(binary_operation_result, line_num);

          if (variable_type != INVALID)
          {
            global_variables[variable_index].type = variable_type;
            global_variables[variable_index].value = binary_operation_result.value;
          }
          else 
            error(line_num, TYPE_INVALID, "Invalid variable value.");
        }

        // Variable doesn't exist
        else
        {
          ResultTokens result_tokens = *GetTokensUntilEOL(&NEXT_TOKEN_1, tokens_count - (i + 1), line_num);

          Token binary_operation_result = ParseBinaryOperation(result_tokens.result_tokens, result_tokens.result_tokens_count, line_num);

          VariableType variable_type = TokenTypeToVariableType(binary_operation_result, line_num);

          if (variable_type != INVALID)
          {
            CreateGlobalVariable(PREVIOUS_TOKEN.value, variable_type, binary_operation_result.value);
          }
          else 
            error(line_num, TYPE_INVALID, "Invalid variable value.");
        }
      }
      else
        error(line_num, SYNTAX_INVALID, "Incomplete assignment (=).");
    }
  }

  TerminateGlobalVariables();
  TerminateFunctions();
  return 0;
}