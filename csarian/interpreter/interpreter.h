// interpreter.h
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>
#include <stdio.h>

#include "../definitions.h"
#include "../token_utils/token_utils.h"

void *Interpreter(Token *tokens, size_t tokens_count, bool in_function, ssize_t current_function,
                  size_t line_num);

#endif