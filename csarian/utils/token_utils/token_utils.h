// token_utils.h
#ifndef TOKENUTILS_H
#define TOKENUTILS_H

#include "csarian/definitions.h"

typedef struct
{
  size_t result_tokens_count;
  Token *result_tokens;
} ResultTokens;

VariableType TokenTypeToVariableType(Token token, ssize_t current_function, size_t line_num);

TokenType VariableTypeToTokenType(VariableType type);

ResultTokens *GetParentTokens(Token *tokens, size_t tokens_count, size_t line_num);

ResultTokens *GetTokensUntilEOL(Token *tokens, size_t tokens_count, size_t line_num);

ResultTokens *GetTokensUntilEOF(Token *tokens, size_t tokens_count, size_t line_num);

ResultTokens *GetTokensUntilOR(Token *tokens, size_t tokens_count, size_t line_num);

#endif