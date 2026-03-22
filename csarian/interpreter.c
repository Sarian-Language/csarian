#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "definitions.h"
#include "error.h"
#include "lexer.h"

#define PREVIOUS_TOKEN tokens[i - 1]
#define CURRENT_TOKEN tokens[i]
#define NEXT_TOKEN_1 tokens[i + 1]
#define NEXT_TOKEN_2 tokens[i + 2]
#define NEXT_TOKEN_3 tokens[i + 3]

// For binary operations
#define OPERAND_A tokens[0]
#define OPERATOR tokens[1]
#define OPERAND_B tokens[2]

Token BinaryOperation(Token *tokens, size_t line_num)
{
  Token result_token;

  if (OPERAND_A.type == TOKEN_IDENTIFIER)
  {
    // Translate it
  }
  if (OPERAND_B.type == TOKEN_IDENTIFIER)
  {
    // Translate it
  }

  // Strings
  if (OPERAND_A.type == TOKEN_STRING || OPERAND_B.type == TOKEN_STRING)
  {
    if (OPERAND_A.type == TOKEN_STRING && OPERAND_B.type == TOKEN_STRING || 
        OPERAND_B.type == TOKEN_INT_LITERAL)
    {
      if (OPERATOR.type == TOKEN_PLUS)
      {
        char *string1 = OPERAND_A.value;
        char *string2 = OPERAND_B.value;

        char *result = malloc(strlen(string1) + strlen(string2) + 1);
        if (!string1) {error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() result.");}

        result[0] = '\0';

        strcat(result, string1);
        strcat(result, string2);

        result_token.type = TOKEN_STRING;
        result_token.value = result;
        result_token.precedence = NO_PRECEDENCE;

        PrintTokens(tokens, line_num);

        return result_token;
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
      error(line_num, TYPE_INVALID, "Operation not allowed between token types.");
    }
  }

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
        if (is_float == true) {float_result = atof(OPERAND_A.value) / atof(OPERAND_B.value);}
        else {int_result = atoi(OPERAND_A.value) / atoi(OPERAND_B.value);}
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
      char buffer[12];
      sprintf(buffer, "%d", int_result);
      result_token.value = strdup(buffer);
      result_token.precedence = NO_PRECEDENCE;
    }
    else 
    {
      result_token.type = TOKEN_FLOAT_LITERAL;
      char buffer[21];
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

int Interpreter(Token *tokens, size_t tokens_count)
{
  size_t line_num = 1;

  size_t i;
  for (i = 0; i < tokens_count; i++)
  {
    if (CURRENT_TOKEN.type == TOKEN_EOL)
    {
      line_num++;
    }
  }

  return 0;
}