// fn.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/definitions.h"
#include "csarian/expressions/binary_operations/binary_operations.h"
#include "csarian/utils/token_utils/token_utils.h"
#include "fn.h"

Function *functions;
size_t functions_size;
size_t functions_count;

void InitFunctions()
{
  functions_size = 32;
  functions_count = 0;

  functions = calloc(functions_size, sizeof(Function));
}

void TerminateFunctions()
{
  free(functions);
  functions = NULL;
}

void AddFunction(char *name, size_t arguments, size_t start, size_t end)
{
  if (functions_count >= functions_size)
  {
    size_t new_size = functions_size * 2;

    Function *tmp = realloc(functions, new_size * sizeof(Function));
    functions = tmp;

    functions_size *= 2;
  }

  functions[functions_count].key = name;
  functions[functions_count].arguments = arguments;
  functions[functions_count].start = start;
  functions[functions_count].end = end;
  functions[functions_count].variables_count = 0;
  functions[functions_count].variables_size = 0;
  functions_count++;
}

ssize_t SearchFunction(char *name)
{
  size_t result;

  for (size_t i = 0; i < functions_count; i++)
  {
    if (strcmp(functions[i].key, name) == 0)
    {
      return i;
    }
  }

  return -1;
}

ResultVariables *GetFunctionArguments(Token *tokens, size_t tokens_count, ssize_t current_function,
                                      size_t line_num)
{
  size_t result_size = 8;
  size_t result_count = 0;

  ResultVariables *result;
  result->result_variables = calloc(result_size, sizeof(Variable));
  if (!result->result_variables)
  {
    error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() function arguments.");
  }

  size_t last_pos = 0;
  for (size_t i = 0; i < tokens_count; i++)
  {
    if (I_CURRENT_TOKEN.type == TOKEN_COMMA || I_CURRENT_TOKEN.type == TOKEN_EOF)
    {
      if (i > last_pos)
      {
        ResultTokens argument_tokens =
          *GetTokensUntilX(TOKEN_COMMA, &tokens[last_pos], tokens_count - last_pos, line_num);
        Token argument_result =
          BinaryOperation(argument_tokens.result_tokens, argument_tokens.result_tokens_count,
                          current_function, line_num);

        last_pos = i + 1;

        if (result_count >= result_size)
        {
          size_t new_size = result_size * 2;

          Variable *tmp = realloc(result->result_variables, new_size * sizeof(Variable));
          if (!tmp)
          {
            error(line_num, MEM_REALLOC_FAILED, "Failed to realloc() function arguments.");
          }

          result->result_variables = tmp;

          result_size *= 2;
        }

        char buffer[INT_TO_STR_SIZE];
        sprintf(buffer, "%ld", result_count);

        result->result_variables[result_count].key = strdup(buffer);
        result->result_variables[result_count].type =
          TokenTypeToVariableType(argument_result, current_function, line_num);
        result->result_variables[result_count].value = argument_result.value;
        result_count++;
      }
      else
      {
        if (!(tokens_count == 1) && (!I_CURRENT_TOKEN.type) == TOKEN_EOF)
        {
          error(line_num, SYNTAX_INVALID, "Invalid comma.");
        }
      }
    }
  }

  result->result_variables_count = result_count;
  return result;
}