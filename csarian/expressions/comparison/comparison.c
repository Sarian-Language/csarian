// comparison.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/definitions.h"
#include "csarian/expressions/binary_operations/binary_operations.h"
#include "csarian/utils/debug/debug.h"
#include "csarian/utils/token_utils/token_utils.h"

static bool Comparison(Token *tokens, size_t tokens_count, ssize_t current_function,
                       size_t line_num)
{
  size_t i;

  for (i = 0; i < tokens_count; i++)
  {
    if (IS_COMPARISON_TOKEN(I_CURRENT_TOKEN.type))
    {
      // Parse left binary operation.
      size_t left_operation_tokens_count = (i + 1);  // i+1 so we can have the TOKEN_EOF.

      // Copy the content of tokens to left_operation_tokens.
      Token *left_operation_tokens = malloc(sizeof(Token) * left_operation_tokens_count);
      for (size_t j = 0; j < left_operation_tokens_count; j++)
        left_operation_tokens[j] = tokens[j];

      left_operation_tokens[i].type = TOKEN_EOF;
      left_operation_tokens[i].value = NULL;
      left_operation_tokens[i].precedence = NO_PRECEDENCE;

      Token left_result = ParseBinaryOperation(left_operation_tokens, left_operation_tokens_count,
                                               current_function, line_num);

      // Parse right binary operation.
      size_t right_operation_tokens_count = tokens_count - (i + 1);
      ResultTokens *right_operation_tokens =
        GetTokensUntilX(TOKEN_EOF, &tokens[i + 1], tokens_count - 1, line_num);

      Token right_result = ParseBinaryOperation(right_operation_tokens->result_tokens,
                                                right_operation_tokens->result_tokens_count,
                                                current_function, line_num);

      // Convert results
      double right;
      double left;

      if (left_result.type == TOKEN_STRING || right_result.type == TOKEN_STRING)
      {
        if (left_result.type == TOKEN_STRING && right_result.type == TOKEN_STRING)
        {
          if (I_CURRENT_TOKEN.type == TOKEN_EQUAL || I_CURRENT_TOKEN.type == TOKEN_NOT_EQUAL)
          {
            if (strcmp(left_result.value, right_result.value) == 0)
            {
              right = 0;
              left = 0;
            }
            else
            {
              right = 1;
              left = 0;
            }
          }
          else
          {
            error(line_num, TYPE_INVALID_OPERATOR,
                  "Only '==' and '!=' comparison operators are allowed in string comparisons.");
          }
        }
        else
        {
          error(line_num, TYPE_INVALID, "Can only compare string to string.");
        }
      }

      else
      {
        if (left_result.type == TOKEN_FLOAT_LITERAL)
        {
          left = atof(left_result.value);
        }
        else
        {
          left = atoi(left_result.value);
        }

        if (right_result.type == TOKEN_FLOAT_LITERAL)
        {
          right = atof(right_result.value);
        }
        else
        {
          right = atoi(right_result.value);
        }
      }

      switch (I_CURRENT_TOKEN.type)
      {
        case TOKEN_EQUAL:
          if (left == right)
          {
            return true;
          }
          break;
        case TOKEN_NOT_EQUAL:
          if (left != right)
          {
            return true;
          }
          break;
        case TOKEN_GREATER:
          if (left > right)
          {
            return true;
          }
          break;
        case TOKEN_GREATER_EQUAL:
          if (left >= right)
          {
            return true;
          }
          break;
        case TOKEN_LESS:
          if (left < right)
          {
            return true;
          }
          break;
        case TOKEN_LESS_EQUAL:
          if (left <= right)
          {
            return true;
          }
          break;

        default:
          break;
      }
    }
  }

  return false;
}

bool ParseComparison(Token *tokens, size_t tokens_count, ssize_t current_function, size_t line_num)
{
  bool left_result = false;
  bool right_result = false;

  bool result = false;

  size_t comparison_tokens_size = 16;
  size_t comparison_tokens_count = 0;
  Token *comparison_tokens = calloc(comparison_tokens_size, sizeof(Token));

  for (size_t i = 0; i < tokens_count; i++)
  {
    if (I_CURRENT_TOKEN.type == TOKEN_OR)
    {
      ResultTokens *right_tokens =
        GetTokensUntilX(TOKEN_OR, &tokens[i + 1], tokens_count - (i + 1), line_num);

      // Add EOF token to the left tokens.
      comparison_tokens[comparison_tokens_count].type = TOKEN_EOF;
      comparison_tokens[comparison_tokens_count].value = NULL;
      comparison_tokens[comparison_tokens_count].precedence = NO_PRECEDENCE;
      comparison_tokens_count++;

      left_result =
        Comparison(comparison_tokens, comparison_tokens_count, current_function, line_num);
      right_result = Comparison(right_tokens->result_tokens, right_tokens->result_tokens_count,
                                current_function, line_num);

      if (left_result == true || right_result == true)
      {
        result = true;
        break;
      }
      else
      {
        result = false;
        break;
      }
    }

    if (I_CURRENT_TOKEN.type == TOKEN_AND)
    {
      ResultTokens *right_tokens =
        GetTokensUntilX(TOKEN_OR, &tokens[i + 1], tokens_count - (i + 1), line_num);

      // Add EOF token to the left tokens.
      comparison_tokens[comparison_tokens_count].type = TOKEN_EOF;
      comparison_tokens[comparison_tokens_count].value = NULL;
      comparison_tokens[comparison_tokens_count].precedence = NO_PRECEDENCE;
      comparison_tokens_count++;

      left_result =
        Comparison(comparison_tokens, comparison_tokens_count, current_function, line_num);
      right_result = Comparison(right_tokens->result_tokens, right_tokens->result_tokens_count,
                                current_function, line_num);

      if (left_result == true && right_result == true)
      {
        result = true;
        break;
      }
      else
      {
        result = false;
        break;
      }
    }

    else if (I_CURRENT_TOKEN.type == TOKEN_EOF)
    {
      comparison_tokens[comparison_tokens_count].type = TOKEN_EOF;
      comparison_tokens[comparison_tokens_count].value = NULL;
      comparison_tokens[comparison_tokens_count].precedence = NO_PRECEDENCE;
      comparison_tokens_count++;

      result = Comparison(comparison_tokens, comparison_tokens_count, current_function, line_num);
      break;
    }

    else
    {
      if (comparison_tokens_size >= comparison_tokens_count)
      {
        size_t new_size = comparison_tokens_size * 2;

        Token *tmp = realloc(comparison_tokens, new_size);

        comparison_tokens = tmp;
        comparison_tokens_size *= 2;
      }

      comparison_tokens[comparison_tokens_count].type = I_CURRENT_TOKEN.type;
      comparison_tokens[comparison_tokens_count].value = I_CURRENT_TOKEN.value;
      comparison_tokens[comparison_tokens_count].precedence = I_CURRENT_TOKEN.precedence;
      comparison_tokens_count++;
    }
  }

  free(comparison_tokens);
  comparison_tokens = NULL;

  return result;
}