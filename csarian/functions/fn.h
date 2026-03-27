// fn.h
#ifndef FN_H
#define FN_H

#include "../definitions.h"

extern Function *functions;

void InitFunctions();

void TerminateFunctions();

void AddFunction(char *name, size_t start, size_t end);

size_t SearchFunction(char *name);

#endif