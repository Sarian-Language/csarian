// interpreter.h
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>

#include "definitions.h"

typedef struct
{
  size_t result_tokens_count;
  Token *result_tokens;
} ResultTokens;

int Interpreter(Token *tokens, size_t tokens_count);

VariableType TokenTypeToVariableType(Token token, size_t line_num);

TokenType VariableTypeToTokenType(VariableType type);

ResultTokens *GetParentTokens(Token *tokens, size_t tokens_count, size_t line_num);

#endif