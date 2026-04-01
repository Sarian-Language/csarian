// interpreter.h
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>

#include "definitions.h"
#include "token_utils/token_utils.h"

int Interpreter(Token *tokens, size_t tokens_count);

#endif