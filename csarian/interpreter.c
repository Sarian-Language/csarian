// interpreter.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary_operations/binary_operations.h"
#include "comparison/comparison.h"
#include "debug/debug.h"
#include "definitions.h"
#include "error_handling/error.h"
#include "functions/fn.h"
#include "global_variables/global_vars.h"
#include "token_utils/token_utils.h"

int Interpreter(Token *tokens, size_t tokens_count)
{
  InitGlobalVariables();
  InitFunctions();

  size_t line_num = 1;

  bool in_block = false;

  // End position of the current block being executed
  ssize_t block_end = -1;

  // Token to return to after executing a block (e.g., function or conditional)
  ssize_t original_pos = -1;

  size_t i;
  for (i = 0; i < tokens_count; i++)
  {
    if (CURRENT_TOKEN.type == TOKEN_EOL)
    {
      line_num++;
    }

    if (in_block == true)
    {
      if (i > block_end)  // Check if we reached the end of the block
      {
        i = original_pos;

        original_pos = -1;
        in_block = false;
        block_end = -1;

        continue;
      }
    }

    if (CURRENT_TOKEN.type == TOKEN_IF)
    {
      if (NEXT_TOKEN_1.type == TOKEN_LPARENT)
      {
        ResultTokens *parent_tokens =
          GetParentTokens(&tokens[i + 1], tokens_count - (i + 1), line_num);

        bool result =
          Comparison(parent_tokens->result_tokens, parent_tokens->result_tokens_count, line_num);

        if (result)
        {
          ssize_t if_block_start = -1;

          for (size_t j = i + 1 + parent_tokens->result_tokens_count + 1; j < tokens_count; j++)
          {
            if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              line_num++;

            else if (J_CURRENT_TOKEN.type == TOKEN_LBRACKET)
            {
              if (if_block_start == -1)
              {
                if_block_start = j;
                break;
              }
            }

            else
              error(line_num, SYNTAX_INVALID, "Expected '{'.");
          }

          i = if_block_start;
          continue;
        }

        else
        {
          size_t depth = 0;
          ssize_t if_block_end = -1;
          bool found_block = false;

          for (size_t j = i + 1 + parent_tokens->result_tokens_count + 1; j < tokens_count; j++)
          {
            if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              line_num++;

            if (J_CURRENT_TOKEN.type == TOKEN_LBRACKET)
            {
              if (found_block == true)
                depth++;
              else
                found_block = true;
            }

            if (J_CURRENT_TOKEN.type == TOKEN_RBRACKET)
            {
              if (depth == 0)
              {
                if_block_end = j;
                break;
              }
              else
                depth--;
            }
          }

          if (if_block_end != -1)
          {
            i = if_block_end;
            continue;
          }
          else
            error(line_num, SYNTAX_INCOMPLETE_BRACKET, "Incomplete brackets inside if block.");
        }
      }

      else
        error(line_num, SYNTAX_INVALID, "Expected '(' after if statement.");
    }

    if (CURRENT_TOKEN.type == TOKEN_FN)
    {
      if (i + 1 <= tokens_count && NEXT_TOKEN_1.type == TOKEN_IDENTIFIER)
      {
        if (i + 2 <= tokens_count && NEXT_TOKEN_2.type == TOKEN_LPARENT)
        {
          ResultTokens *parent_tokens =
            GetParentTokens(&NEXT_TOKEN_2, tokens_count - (i + 2), line_num);

          ssize_t fn_block_start = -1;
          ssize_t fn_block_end = -1;

          size_t depth = 0;

          for (size_t j = i + 2 + parent_tokens->result_tokens_count + 1; j < tokens_count; j++)
          {
            if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              line_num++;

            if (J_CURRENT_TOKEN.type == TOKEN_LBRACKET)
            {
              if (fn_block_start == -1)
              {
                fn_block_start = j + 1;
              }
              else
                depth++;
            }

            if (J_CURRENT_TOKEN.type == TOKEN_RBRACKET)
            {
              if (depth == 0)
              {
                fn_block_end = j - 1;
                break;
              }
              else
                depth--;
            }
          }

          if (fn_block_end == -1)
            error(line_num, SYNTAX_INCOMPLETE_BRACKET, "Incomplete brackets inside function.");

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
        in_block = true;

        continue;
      }
    }

    if (CURRENT_TOKEN.type == TOKEN_DBG_PRINT)
    {
      ResultTokens print_tokens = *GetParentTokens(&tokens[i], tokens_count, line_num);

      Token result_token = ParseBinaryOperation(print_tokens.result_tokens,
                                                print_tokens.result_tokens_count, line_num);

      if (result_token.type != TOKEN_NULL)
      {
        printf("%s\n", result_token.value);
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
              printf("%s\n", (char *)variable.variable_value);
            }
            else
              fprintf(stderr, "Variable not found.\n");
          }
          else
            printf("%s\n", print_tokens.result_tokens->value);
        }
        else
          printf("\n");
      }
    }

    if (CURRENT_TOKEN.type == TOKEN_ASSIGNMENT)
    {
      if (i - 1 >= 0 && i + 1 < tokens_count)  // Make sure we don't underflow/overflow
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
          ResultTokens result_tokens =
            *GetTokensUntilEOL(&NEXT_TOKEN_1, tokens_count - (i + 1), line_num);

          Token binary_operation_result = ParseBinaryOperation(
            result_tokens.result_tokens, result_tokens.result_tokens_count, line_num);

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
          ResultTokens result_tokens =
            *GetTokensUntilEOL(&NEXT_TOKEN_1, tokens_count - (i + 1), line_num);

          Token binary_operation_result = ParseBinaryOperation(
            result_tokens.result_tokens, result_tokens.result_tokens_count, line_num);

          VariableType variable_type = TokenTypeToVariableType(binary_operation_result, line_num);

          if (variable_type != INVALID)
          {
            CreateGlobalVariable(PREVIOUS_TOKEN.value, variable_type,
                                 binary_operation_result.value);
          }
          else
          {
            error(line_num, TYPE_INVALID, "Invalid variable value.");
          }
        }
      }
      else
      {
        error(line_num, SYNTAX_INVALID, "Incomplete assignment (=).");
      }
    }
  }

  TerminateGlobalVariables();
  TerminateFunctions();
  return 0;
}