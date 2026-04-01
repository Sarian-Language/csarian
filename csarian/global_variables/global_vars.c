// global_vars.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../definitions.h"
#include "global_vars.h"

GlobalVariable *global_variables;
size_t global_variables_count;
size_t global_variables_size;

void InitGlobalVariables()
{
  global_variables_size = 32;
  global_variables_count = 0;
  global_variables = calloc(global_variables_size, sizeof(GlobalVariable));
}

void TerminateGlobalVariables()
{
  free(global_variables);
  global_variables = NULL;
}

void CreateGlobalVariable(char *name, VariableType type, void *value)
{
  if (global_variables_count > global_variables_size)
  {
    size_t new_size = global_variables_size * 2;

    GlobalVariable *tmp = realloc(global_variables, sizeof(GlobalVariable) * new_size);
    global_variables = tmp;

    global_variables_size *= 2;
  }

  global_variables[global_variables_count].key = name;
  global_variables[global_variables_count].type = type;
  global_variables[global_variables_count].value = value;
  global_variables_count++;
}

GetGlobalVariableResult GetGlobalVariable(char *name)
{
  GetGlobalVariableResult result;

  for (size_t i = 0; i < global_variables_count; i++)
  {
    if (strcmp(global_variables[i].key, name) == 0)
    {
      result.variable_index = i;
      result.variable_type = global_variables[i].type;
      result.variable_value = global_variables[i].value;
      return result;
    }
  }

  // Variable is not found
  result.variable_index = -1;
  result.variable_value = NULL;
  return result;
}