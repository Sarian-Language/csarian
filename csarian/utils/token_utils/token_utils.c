// token_utils.c
#include <stdio.h>
#include <stdlib.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/definitions.h"
#include "csarian/runtime/variables/global_variables/global_vars.h"
#include "csarian/runtime/variables/local_variables/local_vars.h"
#include "csarian/utils/debug/debug.h"
#include "csarian/utils/token_utils/token_utils.h"

VariableType TokenTypeToVariableType(Token token, ssize_t current_function, size_t line_num)
{
  VariableType result;
  GetGlobalVariableResult global_variable_result;
  GetLocalVariableResult local_variable_result;

  switch (token.type)
  {
    case TOKEN_INT_LITERAL:
      result = INTEGER;
      break;
    case TOKEN_FLOAT_LITERAL:
      result = FLOAT;
      break;
    case TOKEN_STRING:
      result = STRING;
      break;
    case TOKEN_IDENTIFIER:
      global_variable_result = GetGlobalVariable(token.value);

      if (current_function != -1)
      {
        local_variable_result = GetLocalVariable(current_function, token.value);
      }

      if (current_function != -1)
      {
        if (local_variable_result.variable_index == -1 &&
            global_variable_result.variable_index == -1)
          error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");

        if (local_variable_result.variable_index != -1)
        {
          result = local_variable_result.variable_type;
        }
        else
        {
          result = global_variable_result.variable_type;
        }
      }
      else
      {
        if (global_variable_result.variable_index == -1)
          error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");

        result = global_variable_result.variable_type;
      }

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
    case INTEGER:
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

ResultTokens *GetParentTokens(Token *tokens, size_t tokens_count, size_t line_num)
{
  ResultTokens *result = malloc(sizeof(ResultTokens));
  if (!result)
    error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");

  result->result_tokens_count = 0;
  size_t result_tokens_size = 16;

  result->result_tokens = calloc(result_tokens_size, sizeof(Token));
  if (!result->result_tokens)
    error(line_num, MEM_CALLOC_FAILED, "Failed to calloc() result_tokens.");

  size_t depth = 0;

  for (size_t i = 0; i < tokens_count; i++)
  {
    if (I_CURRENT_TOKEN.type == TOKEN_EOF)
    {
      if (result->result_tokens_count >= result_tokens_size)
      {
        size_t new_size = result_tokens_size + 1;

        Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
        if (!tmp)
          error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

        result_tokens_size++;
        result->result_tokens = tmp;
      }

      result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
      result->result_tokens[result->result_tokens_count].value = NULL;
      result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
      result->result_tokens_count++;

      return result;
    }

    else if (I_CURRENT_TOKEN.type == TOKEN_LPARENT)
    {
      for (size_t j = i + 1; j < tokens_count; j++)
      {
        if (J_CURRENT_TOKEN.type == TOKEN_LPARENT)
        {
          depth++;

          if (result->result_tokens_count >= result_tokens_size)
          {
            size_t new_size = result_tokens_size * 2;

            Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
            if (!tmp)
              error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

            result_tokens_size *= 2;
            result->result_tokens = tmp;
          }

          result->result_tokens[result->result_tokens_count].type = J_CURRENT_TOKEN.type;
          result->result_tokens[result->result_tokens_count].value = J_CURRENT_TOKEN.value;
          result->result_tokens[result->result_tokens_count].precedence =
            J_CURRENT_TOKEN.precedence;
          result->result_tokens_count++;
        }

        else if (J_CURRENT_TOKEN.type == TOKEN_RPARENT)
        {
          if (depth == 0)
          {
            if (result->result_tokens_count >= result_tokens_size)
            {
              size_t new_size = result_tokens_size + 1;

              Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
              if (!tmp)
                error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

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
            depth--;

            if (result->result_tokens_count >= result_tokens_size)
            {
              size_t new_size = result_tokens_size * 2;

              Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
              if (!tmp)
                error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

              result_tokens_size *= 2;
              result->result_tokens = tmp;
            }

            result->result_tokens[result->result_tokens_count].type = J_CURRENT_TOKEN.type;
            result->result_tokens[result->result_tokens_count].value = J_CURRENT_TOKEN.value;
            result->result_tokens[result->result_tokens_count].precedence =
              J_CURRENT_TOKEN.precedence;
            result->result_tokens_count++;
          }
        }

        else if (J_CURRENT_TOKEN.type == TOKEN_EOF && depth != 0)
          error(line_num, SYNTAX_INCOMPLETE_PARENT, "Incomplete parents.");

        else
        {
          if (result->result_tokens_count >= result_tokens_size)
          {
            size_t new_size = result_tokens_size * 2;

            Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
            if (!tmp)
              error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

            result_tokens_size *= 2;
            result->result_tokens = tmp;
          }

          result->result_tokens[result->result_tokens_count].type = J_CURRENT_TOKEN.type;
          result->result_tokens[result->result_tokens_count].value = J_CURRENT_TOKEN.value;
          result->result_tokens[result->result_tokens_count].precedence =
            J_CURRENT_TOKEN.precedence;
          result->result_tokens_count++;
        }
      }
    }
  }

  if (result->result_tokens_count >= result_tokens_size)
  {
    size_t new_size = result_tokens_size + 1;

    Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
    if (!tmp)
      error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

    result_tokens_size++;
    result->result_tokens = tmp;
  }

  result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
  result->result_tokens[result->result_tokens_count].value = NULL;
  result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
  result->result_tokens_count++;

  return result;
}

ResultTokens *GetTokensUntilX(TokenType token_type, Token *tokens, size_t tokens_count,
                              size_t line_num)
{
  ResultTokens *result = malloc(sizeof(ResultTokens));
  if (!result)
    error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");

  result->result_tokens_count = 0;
  size_t result_tokens_size = 16;

  result->result_tokens = calloc(result_tokens_size, sizeof(Token));
  if (!result->result_tokens)
    error(line_num, MEM_CALLOC_FAILED, "Failed to calloc() result_tokens.");

  for (size_t i = 0; i < tokens_count; i++)
  {
    if (I_CURRENT_TOKEN.type == TOKEN_EOF)
    {
      if (result->result_tokens_count >= result_tokens_size)
      {
        size_t new_size = result_tokens_size + 1;

        Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
        if (!tmp)
          error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

        result_tokens_size++;
        result->result_tokens = tmp;
      }

      result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
      result->result_tokens[result->result_tokens_count].value = NULL;
      result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
      result->result_tokens_count++;

      return result;
    }

    else if (I_CURRENT_TOKEN.type == token_type)
    {
      if (result->result_tokens_count >= result_tokens_size)
      {
        size_t new_size = result_tokens_size + 1;

        Token *tmp = realloc(result->result_tokens, (new_size) * sizeof(Token));
        if (!tmp)
          error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

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
        if (!tmp)
          error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() result_tokens.");

        result_tokens_size *= 2;
        result->result_tokens = tmp;
      }

      result->result_tokens[result->result_tokens_count].type = I_CURRENT_TOKEN.type;
      result->result_tokens[result->result_tokens_count].value = I_CURRENT_TOKEN.value;
      result->result_tokens[result->result_tokens_count].precedence = I_CURRENT_TOKEN.precedence;
      result->result_tokens_count++;
    }
  }

  result->result_tokens[result->result_tokens_count].type = TOKEN_EOF;
  result->result_tokens[result->result_tokens_count].value = NULL;
  result->result_tokens[result->result_tokens_count].precedence = NO_PRECEDENCE;
  result->result_tokens_count++;

  return result;
}