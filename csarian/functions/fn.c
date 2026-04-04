// fn.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../definitions.h"

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

void AddFunction(char *name, size_t start, size_t end)
{
  if (functions_count >= functions_size)
  {
    size_t new_size = functions_size * 2;

    Function *tmp = realloc(functions, new_size * sizeof(Function));
    functions = tmp;

    functions_size *= 2;
  }

  functions[functions_count].key = name;
  functions[functions_count].start = start;
  functions[functions_count].end = end;
  functions_count++;
}

int SearchFunction(char *name)
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