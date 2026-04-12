// fn.h
#ifndef FN_H
#define FN_H

#include "csarian/definitions.h"

typedef struct
{
  size_t result_variables_count;
  Variable *result_variables;
} ResultVariables;

extern Function *functions;
extern size_t functions_count;

void InitFunctions();

void TerminateFunctions();

void AddFunction(char *name, size_t arguments, size_t start, size_t end);

ssize_t SearchFunction(char *name);

ResultVariables *GetFunctionArguments(Token *tokens, size_t tokens_count, ssize_t current_function,
                                      size_t line_num);

#endif