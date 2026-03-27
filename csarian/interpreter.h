// interpreter.h
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>

#include "definitions.h"

int Interpreter(Token *tokens, size_t tokens_count);

VariableType TokenTypeToVariableType(Token token, size_t line_num);

TokenType VariableTypeToTokenType(VariableType type);

#endif