// debug.c
#include <stdio.h>

#include "csarian/definitions.h"

void PrintTokens(Token *tokens, int numTokens)
{
  for (int i = 0; i < numTokens; i++)
  {
    switch (tokens[i].type)
    {
      case TOKEN_PLUS:
        printf("[%d TOKEN_PLUS] ", i);
        break;
      case TOKEN_MINUS:
        printf("[%d TOKEN_MINUS] ", i);
        break;
      case TOKEN_ASTERISK:
        printf("[%d TOKEN_ASTERISK] ", i);
        break;
      case TOKEN_SLASH:
        printf("[%d TOKEN_SLASH] ", i);
        break;
      case TOKEN_PERCENT:
        printf("[%d TOKEN_PERCENT] ", i);
        break;
      case TOKEN_INT_LITERAL:
        printf("[%d TOKEN_INT_LITERAL (%s)] ", i, tokens[i].value);
        break;
      case TOKEN_FLOAT_LITERAL:
        printf("[%d TOKEN_FLOAT_LITERAL (%s)] ", i, tokens[i].value);
        break;
      case TOKEN_ASSIGNMENT:
        printf("[%d TOKEN_ASSIGNMENT] ", i);
        break;
      case TOKEN_IDENTIFIER:
        printf("[%d TOKEN_IDENTIFIER (%s)] ", i, tokens[i].value);
        break;
      case TOKEN_STRING:
        printf("[%d TOKEN_STRING (%s)] ", i, tokens[i].value);
        break;
      case TOKEN_LPARENT:
        printf("[%d TOKEN_LPARENT] ", i);
        break;
      case TOKEN_RPARENT:
        printf("[%d TOKEN_RPARENT] ", i);
        break;
      case TOKEN_IF:
        printf("[%d TOKEN_IF] ", i);
        break;
      case TOKEN_WHILE:
        printf("[%d TOKEN_WHILE] ", i);
        break;
      case TOKEN_FOR:
        printf("[%d TOKEN_FOR] ", i);
        break;
      case TOKEN_ELSE:
        printf("[%d TOKEN_ELSE] ", i);
        break;
      case TOKEN_LBRACE:
        printf("[%d TOKEN_LBRACE] ", i);
        break;
      case TOKEN_RBRACE:
        printf("[%d TOKEN_RBRACE] ", i);
        break;
      case TOKEN_EQUAL:
        printf("[%d TOKEN_EQUAL] ", i);
        break;
      case TOKEN_NOT_EQUAL:
        printf("[%d TOKEN_NOT_EQUAL] ", i);
        break;
      case TOKEN_LESS:
        printf("[%d TOKEN_LESS] ", i);
        break;
      case TOKEN_LESS_EQUAL:
        printf("[%d TOKEN_LESS_EQUAL] ", i);
        break;
      case TOKEN_GREATER:
        printf("[%d TOKEN_GREATER] ", i);
        break;
      case TOKEN_GREATER_EQUAL:
        printf("[%d TOKEN_GREATER_EQUAL] ", i);
        break;
      case TOKEN_AND:
        printf("[%d TOKEN_AND] ", i);
        break;
      case TOKEN_OR:
        printf("[%d TOKEN_OR] ", i);
        break;
      case TOKEN_EXCLAMATION:
        printf("[%d TOKEN_EXCLAMATION] ", i);
        break;
      case TOKEN_EOF:
        printf("[%d TOKEN_EOF] ", i);
        break;
      case TOKEN_EOL:
        printf("[%d TOKEN_EOL] ", i);
        break;
      case TOKEN_COLON:
        printf("[%d TOKEN_COLON] ", i);
        break;
      case TOKEN_DBG_PRINT:
        printf("[%d TOKEN_DBG_PRINT] ", i);
        break;
      case TOKEN_FN:
        printf("[%d TOKEN_FN] ", i);
        break;
      case TOKEN_IMPORT:
        printf("[%d TOKEN_IMPORT] ", i);
        break;
      case TOKEN_NULL:
        printf("[%d TOKEN_NULL] ", i);
        break;
      case TOKEN_RETURN:
        printf("[%d TOKEN_RETURN] ", i);
        break;
      case TOKEN_GOTO:
        printf("[%d TOKEN_GOTO] ", i);
        break;
      case TOKEN_COMMA:
        printf("[%d TOKEN_COMMA] ", i);
        break;

      default:
        printf("[%d UNKNOWN TOKEN] ", i);
        break;
    }
  }
  printf("\n");
}