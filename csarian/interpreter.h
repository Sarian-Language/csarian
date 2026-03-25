// interpreter
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>

#include "definitions.h"

int Interpreter(Token *tokens, size_t tokens_count);

VariableType token_type_to_variable_type(Token token, size_t line_num);

TokenType variable_type_to_token_type(VariableType type);

#endif