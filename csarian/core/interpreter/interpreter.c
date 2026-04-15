// interpreter.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/core/interpreter/interpreter.h"
#include "csarian/core/lexer/lexer.h"
#include "csarian/definitions.h"
#include "csarian/expressions/binary_operations/binary_operations.h"
#include "csarian/expressions/comparison/comparison.h"
#include "csarian/runtime/functions/fn.h"
#include "csarian/runtime/labels/label.h"
#include "csarian/runtime/variables/global_variables/global_vars.h"
#include "csarian/runtime/variables/local_variables/local_vars.h"
#include "csarian/utils/debug/debug.h"
#include "csarian/utils/token_utils/token_utils.h"

static void IgnoreElseBlock(Token *tokens, size_t tokens_count, size_t *i, size_t *line_num)
{
  bool found_block = false;
  size_t depth = 0;
  ssize_t else_block_end = -1;

  for (size_t j = *i + 1; j < tokens_count; j++)
  {
    if (J_CURRENT_TOKEN.type == TOKEN_EOL)
    {
      line_num++;
    }

    if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
    {
      if (found_block)
      {
        depth++;
      }
      else
      {
        found_block = true;
      }
    }

    if (J_CURRENT_TOKEN.type == TOKEN_RBRACE)
    {
      if (depth == 0)
      {
        else_block_end = j;
        break;
      }
      else
      {
        depth--;
      }
    }
  }

  if (else_block_end != -1)
  {
    *i = else_block_end;
  }
  else
  {
    error(*line_num, SYNTAX_INCOMPLETE_BRACE, "Incomplete braces in else block.");
  }
}

static void HandleFN(Token *tokens, size_t tokens_count, size_t *i, ssize_t current_function,
                     size_t *line_num)
{
  if (*i + 1 <= tokens_count && PTR_I_NEXT_TOKEN_1.type == TOKEN_IDENTIFIER)
  {
    if (*i + 2 <= tokens_count && PTR_I_NEXT_TOKEN_2.type == TOKEN_LPARENT)
    {
      ResultTokens *parent_tokens =
        GetParentTokens(&PTR_I_NEXT_TOKEN_2, tokens_count - (*i + 2), *line_num);

      ssize_t fn_block_start = -1;
      ssize_t fn_block_end = -1;

      size_t depth = 0;

      for (size_t j = *i + 2 + parent_tokens->result_tokens_count + 1; j < tokens_count; j++)
      {
        if (J_CURRENT_TOKEN.type == TOKEN_EOL)
        {
          line_num++;
        }

        if (J_CURRENT_TOKEN.type == TOKEN_FN)
        {
          error(*line_num, SYNTAX_ILLEGAL_FUNCTION,
                "Cannot declare function inside another function.");
        }

        if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
        {
          if (fn_block_start == -1)
          {
            fn_block_start = j + 1;
          }
          else
            depth++;
        }

        if (J_CURRENT_TOKEN.type == TOKEN_RBRACE)
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
        error(*line_num, SYNTAX_INCOMPLETE_BRACE, "Incomplete braces inside function.");

      AddFunction(PTR_I_NEXT_TOKEN_1.value, 0, fn_block_start, fn_block_end);

      // Arguments
      if (parent_tokens->result_tokens_count > 1)
      {
        for (size_t j = 0; j < parent_tokens->result_tokens_count; j++)
        {
          if (parent_tokens->result_tokens[j].type == TOKEN_IDENTIFIER)
          {
            CreateLocalVariable(functions_count - 1, parent_tokens->result_tokens[j].value, INVALID,
                                "NULL");
            functions[functions_count - 1].arguments++;
          }

          else if (parent_tokens->result_tokens[j].type == TOKEN_COMMA)
          {
            // Ignore
          }

          else if (parent_tokens->result_tokens[j].type == TOKEN_EOF)
          {
            break;
          }

          else
          {
            error(*line_num, SYNTAX_INVALID,
                  "Only identifiers and commas are allowed in function arguments.");
          }
        }
      }

      *i = fn_block_end + 1;
    }
    else
    {
      error(*line_num, SYNTAX_INVALID, "Expected '('.");
    }
  }
  else
  {
    error(*line_num, SYNTAX_INVALID, "Expected function name after 'fn'.");
  }
}

static void HandleIdentifier(Token *tokens, size_t tokens_count, size_t *i, ssize_t *block_end,
                             bool *in_function, ssize_t *original_pos, ssize_t *current_function,
                             size_t line_num)
{
  // Search for functions
  ssize_t result = SearchFunction(PTR_I_CURRENT_TOKEN.value);

  if (result != -1)
  {
    // Search for the function call end so we can
    // jump after it after being done with the function.
    for (size_t j = *i; j < tokens_count; j++)
    {
      if (J_CURRENT_TOKEN.type == TOKEN_RPARENT)
      {
        *original_pos = j;
        break;
      }
    }

    // Arguments
    if (PTR_I_NEXT_TOKEN_1.type == TOKEN_LPARENT)
    {
      ResultTokens parent_tokens = *GetParentTokens(&tokens[*i + 1], tokens_count, line_num);

      ResultVariables *arguments =
        GetFunctionArguments(parent_tokens.result_tokens, parent_tokens.result_tokens_count,
                             *current_function, line_num);

      if (arguments->result_variables_count != functions[result].arguments)
      {
        if (arguments->result_variables_count > functions[result].arguments)
          error(line_num, TYPE_INVALID_ARGUMENTS, "Expected less arguments for function call.");

        if (arguments->result_variables_count < functions[result].arguments)
          error(line_num, TYPE_INVALID_ARGUMENTS, "Expected more arguments for function call.");
      }

      for (size_t k = 0; k < functions[result].arguments; k++)
      {
        functions[result].function_variables[k].type = arguments->result_variables[k].type;
        functions[result].function_variables[k].value = arguments->result_variables[k].value;
      }
    }
    else
    {
      error(line_num, SYNTAX_INVALID, "Expected '(' after function call.");
    }

    *i = functions[result].start - 1;
    *block_end = functions[result].end;
    *in_function = true;
    *current_function = result;
  }
  else
  {
    if (in_function)
    {
      if (GetLocalVariable(*current_function, PTR_I_CURRENT_TOKEN.value).variable_index == -1 &&
          GetGlobalVariable(PTR_I_CURRENT_TOKEN.value).variable_index == -1)
      {
        if (PTR_I_NEXT_TOKEN_1.type != TOKEN_ASSIGNMENT && PTR_I_NEXT_TOKEN_1.type != TOKEN_COLON)
        {
          error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");
        }
      }
    }

    else
    {
      if (GetGlobalVariable(PTR_I_CURRENT_TOKEN.value).variable_index == -1)
      {
        if (PTR_I_NEXT_TOKEN_1.type != TOKEN_ASSIGNMENT && PTR_I_NEXT_TOKEN_1.type != TOKEN_COLON)
        {
          error(line_num, IDENTIFIER_UNKNOWN, "Unknown identifier.");
        }
      }
    }
  }
}

static void HandleDebugPrint(Token *tokens, size_t tokens_count, size_t i, ssize_t current_function,
                             size_t line_num)
{
  ResultTokens print_tokens = *GetParentTokens(&tokens[i], tokens_count, line_num);

  Token result_token = ParseBinaryOperation(
    print_tokens.result_tokens, print_tokens.result_tokens_count, current_function, line_num);

  if (result_token.type != TOKEN_NULL)
  {
    printf("%s\n", result_token.value);
  }
  else
  {
    printf("\n");
  }
}

static void HandleAssignment(Token *tokens, size_t tokens_count, size_t i, ssize_t current_function,
                             bool in_function, size_t line_num)
{
  if (i - 1 >= 0 && i + 1 < tokens_count)  // Make sure we don't underflow/overflow
  {
    ssize_t global_variable_index = -1;
    ssize_t local_variable_index = -1;

    if (I_PREVIOUS_TOKEN.type == TOKEN_IDENTIFIER)
    {
      global_variable_index = GetGlobalVariable(I_PREVIOUS_TOKEN.value).variable_index;

      if (in_function)
      {
        local_variable_index =
          GetLocalVariable(current_function, I_PREVIOUS_TOKEN.value).variable_index;
      }
    }
    else
      error(line_num, SYNTAX_INVALID, "Expected identifier before '='.");

    if (in_function)
    {
      ResultTokens result_tokens =
        *GetTokensUntilX(TOKEN_EOL, &I_NEXT_TOKEN_1, tokens_count - (i + 1), line_num);

      if (local_variable_index != -1)
      {
        Token binary_operation_result =
          ParseBinaryOperation(result_tokens.result_tokens, result_tokens.result_tokens_count,
                               current_function, line_num);

        VariableType variable_type =
          TokenTypeToVariableType(binary_operation_result, current_function, line_num);

        if (variable_type != INVALID)
        {
          functions[current_function].function_variables[local_variable_index].type = variable_type;
          functions[current_function].function_variables[local_variable_index].value =
            binary_operation_result.value;
        }
        else
          error(line_num, TYPE_INVALID, "Invalid variable value.");
      }
      else
      {
        Token binary_operation_result =
          ParseBinaryOperation(result_tokens.result_tokens, result_tokens.result_tokens_count,
                               current_function, line_num);

        VariableType variable_type =
          TokenTypeToVariableType(binary_operation_result, current_function, line_num);

        if (variable_type != INVALID)
        {
          CreateLocalVariable(current_function, I_PREVIOUS_TOKEN.value, variable_type,
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
      ResultTokens result_tokens =
        *GetTokensUntilX(TOKEN_EOL, &I_NEXT_TOKEN_1, tokens_count - (i + 1), line_num);

      if (global_variable_index != -1)
      {
        Token binary_operation_result =
          ParseBinaryOperation(result_tokens.result_tokens, result_tokens.result_tokens_count,
                               current_function, line_num);

        VariableType variable_type =
          TokenTypeToVariableType(binary_operation_result, current_function, line_num);

        if (variable_type != INVALID)
        {
          global_variables[global_variable_index].type = variable_type;
          global_variables[global_variable_index].value = binary_operation_result.value;
        }
        else
          error(line_num, TYPE_INVALID, "Invalid variable value.");
      }
      else
      {
        Token binary_operation_result =
          ParseBinaryOperation(result_tokens.result_tokens, result_tokens.result_tokens_count,
                               current_function, line_num);

        VariableType variable_type =
          TokenTypeToVariableType(binary_operation_result, current_function, line_num);

        if (variable_type != INVALID)
        {
          CreateGlobalVariable(I_PREVIOUS_TOKEN.value, variable_type,
                               binary_operation_result.value);
        }
        else
        {
          error(line_num, TYPE_INVALID, "Invalid variable value.");
        }
      }
    }
  }
  else
  {
    error(line_num, SYNTAX_INVALID, "Incomplete assignment (=).");
  }
}

static void HandleLabel(Token *tokens, size_t i, size_t line_num)
{
  if (i - 1 >= 0 && I_PREVIOUS_TOKEN.type == TOKEN_IDENTIFIER)
  {
    CreateLabel(I_PREVIOUS_TOKEN.value, i);
  }
  else
  {
    error(line_num, SYNTAX_INVALID, "Invalid or missing label name.");
  }
}

void *Interpreter(Token *tokens, size_t tokens_count, bool in_function, ssize_t current_function,
                  size_t line_num)
{
  void *Return = NULL;

  InitGlobalVariables();
  InitFunctions();
  InitLabels();

  bool in_while = false;
  ResultTokens *while_comparison_tokens;
  ssize_t while_block_start = -1;

  // End position of the current block being executed
  ssize_t block_end = -1;

  ssize_t original_pos = -1;  // Token to return to after executing a block.

  size_t i;
  for (i = 0; i < tokens_count; i++)
  {
    if (I_CURRENT_TOKEN.type == TOKEN_EOF)
    {
      break;
    }

    if (I_CURRENT_TOKEN.type == TOKEN_EOL)
    {
      if (!in_function)
      {
        line_num++;
      }
    }

    if (in_function == true)
    {
      if (i != -1 && i > block_end)  // Check if we reached the end of the block
      {
        i = original_pos;

        original_pos = -1;
        in_function = false;
        current_function = -1;
        block_end = -1;

        continue;
      }
    }

    if (in_while == true)
    {
      if (i > block_end)
      {
        if (ParseComparison(while_comparison_tokens->result_tokens,
                            while_comparison_tokens->result_tokens_count, current_function,
                            line_num))
        {
          i = while_block_start;
        }
        else
        {
          i = block_end;
          block_end = -1;
          in_while = false;
        }

        continue;
      }
    }

    // Ignores else blocks, as they are handled in the conditionals and would cause unwanted
    // execution.
    if (I_CURRENT_TOKEN.type == TOKEN_ELSE)
    {
      IgnoreElseBlock(tokens, tokens_count, &i, &line_num);
      continue;
    }

    if (I_CURRENT_TOKEN.type == TOKEN_FN)
    {
      HandleFN(tokens, tokens_count, &i, current_function, &line_num);
      continue;
    }

    if (I_CURRENT_TOKEN.type == TOKEN_IDENTIFIER)
    {
      HandleIdentifier(tokens, tokens_count, &i, &block_end, &in_function, &original_pos,
                       &current_function, line_num);
      continue;
    }

    if (I_CURRENT_TOKEN.type == TOKEN_ASSIGNMENT)
    {
      HandleAssignment(tokens, tokens_count, i, current_function, in_function, line_num);
    }

    if (I_CURRENT_TOKEN.type == TOKEN_DBG_PRINT)
    {
      HandleDebugPrint(tokens, tokens_count, i, current_function, line_num);
    }

    if (I_CURRENT_TOKEN.type == TOKEN_COLON)
    {
      HandleLabel(tokens, i, line_num);
    }

    if (I_CURRENT_TOKEN.type == TOKEN_IMPORT)
    {
      if (I_CURRENT_TOKEN.type == TOKEN_IMPORT)
      {
        if (I_NEXT_TOKEN_1.type == TOKEN_STRING)
        {
          FILE *file = fopen(I_NEXT_TOKEN_1.value, "rb");
          if (!file)
          {
            perror("[Main] Error opening import file");
            exit(1);
          }

          if (fseek(file, 0, SEEK_END) != 0)
          {
            perror("[Main] Error seeking import file end");
            fclose(file);
            exit(1);
          }

          long filesize = ftell(file);
          if (filesize < 0)
          {
            perror("[Interpreter] Error getting import file size");
            fclose(file);
            exit(1);
          }
          fseek(file, 0, SEEK_SET);

          char *code = malloc((size_t)filesize + 1);
          if (!code)
          {
            fclose(file);
            error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() code.");
          }

          size_t bytes_read = fread(code, 1, (size_t)filesize, file);
          if (bytes_read != (size_t)filesize)
          {
            perror("[Interpreter] Error reading file completely");
            free(code);
            fclose(file);
            exit(1);
          }

          code[filesize] = '\0';
          fclose(file);

          ResultTokens import_tokens = Lexer(code);

          size_t insert_pos = i;
          size_t remove_count = 2;

          // Determine how many tokens to insert (excluding TOKEN_EOF)
          size_t tokens_to_insert = import_tokens.result_tokens_count;
          if (tokens_to_insert > 0 &&
              import_tokens.result_tokens[tokens_to_insert - 1].type == TOKEN_EOF)
          {
            tokens_to_insert--;
          }

          // Resize the tokens array
          tokens_count = tokens_count - remove_count + tokens_to_insert;
          tokens = realloc(tokens, sizeof(Token) * tokens_count);
          if (!tokens)
          {
            free(code);
            error(line_num, MEM_REALLOC_FAILED, "Failed to realloc tokens array");
          }

          // Move the subsequent tokens forward
          memmove(&tokens[insert_pos + tokens_to_insert], &tokens[insert_pos + remove_count],
                  sizeof(Token) * (tokens_count - insert_pos - tokens_to_insert));

          // Copy imported tokens
          for (size_t j = 0; j < tokens_to_insert; j++)
          {
            tokens[insert_pos + j] = import_tokens.result_tokens[j];
          }

          free(code);

          i = insert_pos - 1;
          continue;
        }
      }
    }

    // Control Flow
    if (I_CURRENT_TOKEN.type == TOKEN_IF)
    {
      if (I_NEXT_TOKEN_1.type == TOKEN_LPARENT)
      {
        ResultTokens *parent_tokens =
          GetParentTokens(&tokens[i + 1], tokens_count - (i + 1), line_num);

        bool result =
          ParseComparison(parent_tokens->result_tokens, parent_tokens->result_tokens_count,
                          current_function, line_num);

        if (result)
        {
          ssize_t if_block_start = -1;

          for (size_t j = i + 1 + parent_tokens->result_tokens_count + 1; j < tokens_count; j++)
          {
            if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              line_num++;

            else if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
            {
              if (if_block_start == -1)
              {
                if_block_start = j;
                break;
              }
            }

            else
            {
              error(line_num, SYNTAX_INVALID, "Expected '{'.");
            }
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

            if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
            {
              if (found_block == true)
                depth++;
              else
                found_block = true;
            }

            if (J_CURRENT_TOKEN.type == TOKEN_RBRACE)
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
            ssize_t found_else = -1;

            for (size_t j = if_block_end + 1; j < tokens_count; j++)
            {
              if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              {
                line_num++;
              }

              else if (J_CURRENT_TOKEN.type == TOKEN_ELSE)
              {
                found_else = true;
              }

              else if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
              {
                if (found_else)
                {
                  i = j;
                  break;
                }
              }

              else
              {
                i = if_block_end;
                break;
              }
            }

            continue;
          }
          else
          {
            error(line_num, SYNTAX_INCOMPLETE_BRACE, "Incomplete braces inside if block.");
          }
        }
      }

      else
      {
        error(line_num, SYNTAX_INVALID, "Expected '(' after if statement.");
      }
    }

    if (I_CURRENT_TOKEN.type == TOKEN_WHILE)
    {
      if (I_NEXT_TOKEN_1.type == TOKEN_LPARENT)
      {
        while_comparison_tokens = GetParentTokens(&tokens[i + 1], tokens_count - (i + 1), line_num);

        bool result =
          ParseComparison(while_comparison_tokens->result_tokens,
                          while_comparison_tokens->result_tokens_count, current_function, line_num);

        if (result)
        {
          while_block_start = -1;

          size_t depth = 0;
          ssize_t while_block_end = -1;
          bool found_block = false;

          for (size_t j = i + 1 + while_comparison_tokens->result_tokens_count + 1;
               j < tokens_count; j++)
          {
            if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              line_num++;

            else if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
            {
              if (found_block == false)
              {
                found_block = true;
                while_block_start = j;
              }
              else
                depth++;
            }

            else if (J_CURRENT_TOKEN.type == TOKEN_RBRACE)
            {
              if (depth == 0)
              {
                while_block_end = j;
                break;
              }
              else
                depth--;
            }
          }

          if (while_block_start != -1)
          {
            i = while_block_start;

            if (while_block_end != -1)
            {
              block_end = while_block_end;
            }
            else
            {
              error(line_num, SYNTAX_INCOMPLETE_BRACE, "Incomplete braces at while block.");
            }
          }
          else
          {
            error(line_num, SYNTAX_INVALID, "Expected '{' at while loop.");
          }

          in_while = true;
          continue;
        }

        else
        {
          size_t depth = 0;
          ssize_t while_block_end = -1;
          bool found_block = false;

          for (size_t j = i + 1 + while_comparison_tokens->result_tokens_count + 1;
               j < tokens_count; j++)
          {
            if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              line_num++;

            if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
            {
              if (found_block == true)
                depth++;
              else
                found_block = true;
            }

            if (J_CURRENT_TOKEN.type == TOKEN_RBRACE)
            {
              if (depth == 0)
              {
                while_block_end = j;
                break;
              }
              else
                depth--;
            }
          }

          if (while_block_end != -1)
          {
            ssize_t found_else = -1;

            for (size_t j = while_block_end + 1; j < tokens_count; j++)
            {
              if (J_CURRENT_TOKEN.type == TOKEN_EOL)
              {
                line_num++;
              }

              else if (J_CURRENT_TOKEN.type == TOKEN_ELSE)
              {
                found_else = true;
              }

              else if (J_CURRENT_TOKEN.type == TOKEN_LBRACE)
              {
                if (found_else)
                {
                  i = j;
                  break;
                }
              }

              else
              {
                i = while_block_end;
                break;
              }
            }

            continue;
          }
          else
          {
            error(line_num, SYNTAX_INCOMPLETE_BRACE, "Incomplete braces inside while block.");
          }
        }
      }

      else
      {
        error(line_num, SYNTAX_INVALID, "Expected '(' after while statement.");
      }
    }

    if (I_CURRENT_TOKEN.type == TOKEN_GOTO)
    {
      if (I_NEXT_TOKEN_1.type == TOKEN_IDENTIFIER)
      {
        ssize_t label_index = GetLabel(I_NEXT_TOKEN_1.value);

        if (label_index != -1)
        {
          i = labels[label_index].start;
          continue;
        }
        else
        {
          error(line_num, SYNTAX_INVALID, "Unknown 'goto' label.");
        }
      }
      else
      {
        error(line_num, SYNTAX_INVALID, "Missing label name for 'goto'.");
      }
    }
  }

  TerminateGlobalVariables();
  TerminateFunctions();
  TerminateLabels();

  return Return;
}