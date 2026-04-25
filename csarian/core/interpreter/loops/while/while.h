// while.h
#ifndef WHILE_H
#define WHILE_H

#include "csarian/definitions.h"

extern While *while_loops;
extern size_t while_loops_count;

void InitWhileLoops();

void TerminateWhileLoops();

void AddWhileLoop(size_t while_start, size_t while_end, Token *comparison_tokens,
                  size_t comparison_tokens_count);

#endif