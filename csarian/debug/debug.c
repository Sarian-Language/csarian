// debug.c
#include <stdio.h>

#include "../definitions.h"

void PrintTokens(Token *tokens, int numTokens)
{
  for (int i = 0; i < numTokens; i++)
  {
    switch (tokens[i].type)
    {
      case TOKEN_PLUS:
        printf("[TOKEN_PLUS] ");
        break;
      case TOKEN_MINUS:
        printf("[TOKEN_MINUS] ");
        break;
      case TOKEN_ASTERISK:
        printf("[TOKEN_ASTERISK] ");
        break;
      case TOKEN_SLASH:
        printf("[TOKEN_SLASH] ");
        break;
      case TOKEN_PERCENT:
        printf("[TOKEN_PERCENT] ");
        break;
      case TOKEN_INT_LITERAL:
        printf("[TOKEN_INT_LITERAL (%s)] ", tokens[i].value);
        break;
      case TOKEN_FLOAT_LITERAL:
        printf("[TOKEN_FLOAT_LITERAL (%s)] ", tokens[i].value);
        break;
      case TOKEN_ASSIGNMENT:
        printf("[TOKEN_ASSIGNMENT] ");
        break;
      case TOKEN_IDENTIFIER:
        printf("[TOKEN_IDENTIFIER (%s)] ", tokens[i].value);
        break;
      case TOKEN_STRING:
        printf("[TOKEN_STRING (%s)] ", tokens[i].value);
        break;
      case TOKEN_LPARENT:
        printf("[TOKEN_LPARENT] ");
        break;
      case TOKEN_RPARENT:
        printf("[TOKEN_RPARENT] ");
        break;
      case TOKEN_IF:
        printf("[TOKEN_IF] ");
        break;
      case TOKEN_WHILE:
        printf("[TOKEN_WHILE] ");
        break;
      case TOKEN_FOR:
        printf("[TOKEN_FOR] ");
        break;
      case TOKEN_ELSE:
        printf("[TOKEN_ELSE] ");
        break;
      case TOKEN_LBRACE:
        printf("[TOKEN_LBRACE] ");
        break;
      case TOKEN_RBRACE:
        printf("[TOKEN_RBRACE] ");
        break;
      case TOKEN_EQUAL:
        printf("[TOKEN_EQUAL] ");
        break;
      case TOKEN_NOT_EQUAL:
        printf("[TOKEN_NOT_EQUAL] ");
        break;
      case TOKEN_LESS:
        printf("[TOKEN_LESS] ");
        break;
      case TOKEN_LESS_EQUAL:
        printf("[TOKEN_LESS_EQUAL] ");
        break;
      case TOKEN_GREATER:
        printf("[TOKEN_GREATER] ");
        break;
      case TOKEN_GREATER_EQUAL:
        printf("[TOKEN_GREATER_EQUAL] ");
        break;
      case TOKEN_AND:
        printf("[TOKEN_AND] ");
        break;
      case TOKEN_OR:
        printf("[TOKEN_OR] ");
        break;
      case TOKEN_EXCLAMATION:
        printf("[TOKEN_EXCLAMATION] ");
        break;
      case TOKEN_EOF:
        printf("[TOKEN_EOF] ");
        break;
      case TOKEN_EOL:
        printf("[TOKEN_EOL] ");
        break;
      case TOKEN_COLON:
        printf("[TOKEN_COLON] ");
        break;
      case TOKEN_DBG_PRINT:
        printf("[TOKEN_DBG_PRINT] ");
        break;
      case TOKEN_FN:
        printf("[TOKEN_FN] ");
        break;
      case TOKEN_IMPORT:
        printf("[TOKEN_IMPORT] ");
        break;
      case TOKEN_NULL:
        printf("[TOKEN_NULL] ");
        break;
      case TOKEN_RETURN:
        printf("[TOKEN_RETURN] ");
        break;
      case TOKEN_GOTO:
        printf("[TOKEN_GOTO] ");
        break;
      case TOKEN_COMMA:
        printf("[TOKEN_COMMA] ");
        break;

      default:
        printf("[UNKNOWN TOKEN] ");
        break;
    }
  }
  printf("\n");
}