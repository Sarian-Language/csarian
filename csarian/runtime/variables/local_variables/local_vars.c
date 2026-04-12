// local_vars.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/definitions.h"
#include "csarian/runtime/functions/fn.h"
#include "local_vars.h"

void CreateLocalVariable(size_t function_index, char *key, VariableType type, char *value)
{
  if (functions[function_index].variables_size == 0)
  {
    functions[function_index].variables_size = 16;

    functions[function_index].function_variables =
      calloc(functions[function_index].variables_size, sizeof(Variable));
    if (!functions[function_index].function_variables)
    {
      error(0, MEM_CALLOC_FAILED, "Failed to calloc() function local variables.");
    }
  }

  if (functions[function_index].variables_count >= functions[function_index].variables_size)
  {
    size_t new_size = functions[function_index].variables_size * 2;

    Variable *tmp =
      realloc(functions[function_index].function_variables, new_size * sizeof(Variable));
    if (!tmp)
    {
      error(0, MEM_REALLOC_FAILED, "Failed to realloc() function local variables.");
    }

    functions[function_index].function_variables = tmp;

    functions[function_index].variables_size *= 2;
  }

  size_t variables_count = functions[function_index].variables_count;

  functions[function_index].function_variables[variables_count].key = key;
  functions[function_index].function_variables[variables_count].type = type;
  functions[function_index].function_variables[variables_count].value = value;
  functions[function_index].variables_count++;
}

GetLocalVariableResult GetLocalVariable(size_t function_index, char *name)
{
  GetLocalVariableResult result;

  for (size_t i = 0; i < functions[function_index].variables_count; i++)
  {
    if (strcmp(functions[function_index].function_variables[i].key, name) == 0)
    {
      result.variable_index = i;
      result.variable_type = functions[function_index].function_variables[i].type;
      result.variable_value = functions[function_index].function_variables[i].value;
      return result;
    }
  }

  // Variable is not found
  result.variable_index = -1;
  result.variable_value = NULL;
  result.variable_type = INVALID;
  return result;
}