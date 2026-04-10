// local_vars.h
#ifndef LOCALVARS_H
#define LOCALVARS_H

#include "../definitions.h"

typedef struct
{
  ssize_t variable_index;
  void *variable_value;
  VariableType variable_type;
} GetLocalVariableResult;

void CreateLocalVariable(size_t function_index, char *key, VariableType type, char *value);

GetLocalVariableResult GetLocalVariable(size_t function_index, char *name);

#endif