// comparison.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../binary_operations/binary_operations.h"
#include "../debug/debug.h"
#include "../definitions.h"
#include "../token_utils/token_utils.h"

bool Comparison(Token *tokens, size_t tokens_count, size_t line_num)
{
  size_t i;

  for (i = 0; i < tokens_count; i++)
  {
    if (IS_COMPARISON_TOKEN(CURRENT_TOKEN.type))
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

      Token left_result =
        ParseBinaryOperation(left_operation_tokens, left_operation_tokens_count, line_num);

      // Parse right binary operation.
      size_t right_operation_tokens_count = tokens_count - (i + 1);
      ResultTokens *right_operation_tokens =
        GetTokensUntilEOF(&tokens[i + 1], tokens_count - 1, line_num);

      Token right_result =
        ParseBinaryOperation(right_operation_tokens->result_tokens,
                             right_operation_tokens->result_tokens_count, line_num);

      // Convert results
      double right;
      double left;

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

      switch (CURRENT_TOKEN.type)
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
