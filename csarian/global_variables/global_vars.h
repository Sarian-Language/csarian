// global_vars.h
#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "../definitions.h"

typedef struct
{
  ssize_t variable_index;
  void *variable_value;
  VariableType variable_type;
} GetGlobalVariableResult;

void InitGlobalVariables();

void TerminateGlobalVariables();

void CreateGlobalVariable(char *name, VariableType type, void *value);

GetGlobalVariableResult GetGlobalVariable(char *name);

extern Variable *global_variables;

#endif