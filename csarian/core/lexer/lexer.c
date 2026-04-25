// lexer.c
#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/definitions.h"
#include "csarian/utils/debug/debug.h"
#include "csarian/utils/token_utils/token_utils.h"

size_t current_token;
size_t tokens_len;
Token *tokens;

void InitTokens()
{
  tokens_len = 32;
  tokens = calloc(tokens_len, sizeof(Token));
  current_token = 0;
}

void AddToken(TokenType token_type, char *token_value, TokenPrecedence token_precedence)
{
  if (current_token + 1 >= tokens_len)
  {
    size_t new_len = tokens_len * 2;
    Token *tmp = realloc(tokens, sizeof(Token) * new_len);
    tokens = tmp;

    tokens_len = new_len;
  }

  tokens[current_token] = (Token){token_type, token_value, token_precedence};
  current_token++;
}

ResultTokens Lexer(char *code)
{
  ResultTokens result_tokens;

  int current_line = 1;

  InitTokens();

  size_t code_len = strlen(code);
  for (size_t i = 0; i < code_len + 1; i++)
  {
    char current_char = code[i];
    char next_char = code[i + 1];

    if (i == code_len)  // We reached the end of the code.
    {
      AddToken(TOKEN_EOF, NULL, NO_PRECEDENCE);
      break;
    }

    else if (current_char == '\n')
    {
      current_line++;
      AddToken(TOKEN_EOL, NULL, NO_PRECEDENCE);
    }

    // Comments
    else if (current_char == '/' && next_char == '/')
    {
      i += 2;

      while (i < code_len && code[i] != '\n')
        i++;

      continue;
    }

    // Strings
    else if (current_char == '"' || current_char == '\'')
    {
      size_t string_len = 16;

      char *string_value = calloc(string_len, sizeof(char));
      if (!string_value)
      {
        error(current_line, MEM_CALLOC_FAILED, "Failed to calloc() string value.");
      }

      size_t pos = 0;

      for (size_t j = i + 1; j < code_len; j++)
      {
        if (code[j] == '"' || code[j] == '\'')
        {
          AddToken(TOKEN_STRING, strdup(string_value), NO_PRECEDENCE);

          free(string_value);
          string_value = NULL;

          i = j;

          break;
        }
        else
        {
          // Continue to add string characters if we don't find the end.
          if (pos + 1 >= string_len)
          {
            size_t new_len = string_len *= 2;

            char *tmp = realloc(string_value, new_len);
            if (!tmp)
            {
              error(current_line, MEM_REALLOC_FAILED, "Failed to realloc() string value.");
            }

            string_value = tmp;

            string_len = new_len;
          }

          string_value[pos++] = code[j];
          string_value[pos] = '\0';
        }
      }
    }

    // Identifiers / Keywords
    else if (isalpha(current_char) || current_char == '_')
    {
      size_t identifier_len = 16;

      char *identifier = calloc(identifier_len, sizeof(char));
      if (!identifier)
      {
        error(current_line, MEM_CALLOC_FAILED, "Failed to calloc() identifier array.");
      }

      identifier[0] = current_char;

      size_t pos = 1;

      for (size_t j = i + 1; j <= code_len;
           j++)  // Iterate until we find the end of the identifier.
      {
        if (isalpha(code[j]) || isdigit(code[j]) || code[j] == '_')
        {
          if (pos + 1 >= identifier_len)
          {
            size_t new_len = identifier_len * 2;

            char *tmp = realloc(identifier, new_len);
            if (!tmp)
            {
              error(current_line, MEM_REALLOC_FAILED, "Failed to realloc() identifier array.");
            }

            identifier = tmp;
            identifier_len *= 2;
          }

          identifier[pos++] = code[j];
          identifier[pos + 1] = '\0';
        }
        else
        {
          i = j - 1;

          // Keywords
          if (strcmp(identifier, "and") == 0)
            AddToken(TOKEN_AND, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "or") == 0)
            AddToken(TOKEN_OR, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "if") == 0)
            AddToken(TOKEN_IF, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "else") == 0)
            AddToken(TOKEN_ELSE, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "while") == 0)
            AddToken(TOKEN_WHILE, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "for") == 0)
            AddToken(TOKEN_FOR, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "printd") == 0)
            AddToken(TOKEN_DBG_PRINT, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "fn") == 0)
            AddToken(TOKEN_FN, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "import") == 0)
            AddToken(TOKEN_IMPORT, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "return") == 0)
            AddToken(TOKEN_RETURN, NULL, NO_PRECEDENCE);

          else if (strcmp(identifier, "goto") == 0)
            AddToken(TOKEN_GOTO, NULL, NO_PRECEDENCE);

          // Not a keyword, adding normal identifier token.
          else
          {
            AddToken(TOKEN_IDENTIFIER, strdup(identifier), NO_PRECEDENCE);
          }

          free(identifier);
          identifier = NULL;

          break;
        }
      }
    }

    // Numbers
    else if (isdigit(current_char))
    {
      bool is_float = false;
      size_t number_len = 16;

      char *number = calloc(number_len, sizeof(char));
      if (!number)
      {
        error(current_line, MEM_CALLOC_FAILED, "Failed to calloc() digit value.");
      }

      number[0] = current_char;

      size_t pos = 1;

      for (size_t j = i + 1; j <= code_len; j++)  // Iterate until we find the end of the number
      {
        if (isdigit(code[j]))
        {
          if (pos + 1 >= number_len)
          {
            size_t new_len = number_len * 2;

            char *tmp = realloc(number, new_len);
            if (!tmp)
            {
              error(current_line, MEM_REALLOC_FAILED, "Failed to realloc() digit value.");
            }

            number = tmp;
            number_len *= 2;
          }

          number[pos++] = code[j];
          number[pos + 1] = '\0';
        }
        else if (code[j] == '.')  // Detects decimal number
        {
          if (is_float == true)
          {
            error(current_line, SYNTAX_INVALID, "More than one decimal point.");
          }

          size_t new_len = number_len * 2;
          is_float = true;

          if (pos + 1 >= number_len)
          {
            char *tmp = realloc(number, new_len);
            if (!tmp)
            {
              error(current_line, MEM_REALLOC_FAILED, "Failed to realloc() digit value.");
            }

            number = tmp;
            number_len *= 2;
          }

          number[pos++] = code[j];
          number[pos + 1] = '\0';
        }
        else
        {
          i = j - 1;

          if (!is_float)
          {
            AddToken(TOKEN_INT_LITERAL, strdup(number), NO_PRECEDENCE);
          }
          else
          {
            AddToken(TOKEN_FLOAT_LITERAL, strdup(number), NO_PRECEDENCE);
          }

          break;
        }
      }

      free(number);
      number = NULL;
    }

    // COMMA
    else if (current_char == ',')
    {
      AddToken(TOKEN_COMMA, NULL, NO_PRECEDENCE);
    }

    // COLON
    else if (current_char == ':')
    {
      AddToken(TOKEN_COLON, NULL, NO_PRECEDENCE);
    }

    // SEMICOLON (EOL)
    else if (current_char == ';')
    {
      current_line++;
      AddToken(TOKEN_EOL, NULL, NO_PRECEDENCE);
    }

    // PLUS operator
    else if (current_char == '+')
    {
      AddToken(TOKEN_PLUS, NULL, PLUS_PRECEDENCE);
    }
    // MINUS operator
    else if (current_char == '-')
    {
      AddToken(TOKEN_MINUS, NULL, MINUS_PRECEDENCE);
    }
    // PERCENT operator
    else if (current_char == '%')
    {
      AddToken(TOKEN_PERCENT, NULL, PERCENT_PRECEDENCE);
    }
    // ASTERISK operator
    else if (current_char == '*')
    {
      AddToken(TOKEN_ASTERISK, NULL, ASTERISK_PRECEDENCE);
    }
    // SLASH operator
    else if (current_char == '/' && next_char != '/')
    {
      AddToken(TOKEN_SLASH, NULL, SLASH_PRECEDENCE);
    }
    // ASSIGNMENT / EQUAL operator
    else if (current_char == '=')
    {
      if (next_char != '=')
      {
        AddToken(TOKEN_ASSIGNMENT, NULL, NO_PRECEDENCE);
      }
      else
      {
        AddToken(TOKEN_EQUAL, NULL, NO_PRECEDENCE);
        i++;
      }
    }
    // GREATER / GREATER OR EQUAL operator
    else if (current_char == '>')
    {
      if (next_char != '=')
      {
        AddToken(TOKEN_GREATER, NULL, NO_PRECEDENCE);
      }
      else
      {
        AddToken(TOKEN_GREATER_EQUAL, NULL, NO_PRECEDENCE);
        i++;
      }
    }
    // LESS / LESS OR EQUAL operator
    else if (current_char == '<')
    {
      if (next_char != '=')
      {
        AddToken(TOKEN_LESS, NULL, NO_PRECEDENCE);
      }
      else
      {
        AddToken(TOKEN_LESS_EQUAL, NULL, NO_PRECEDENCE);
        i++;
      }
    }
    // EXCLAMATION MARK / NOT EQUAL operator
    else if (current_char == '!')
    {
      if (next_char != '=')
      {
        AddToken(TOKEN_EXCLAMATION, NULL, NO_PRECEDENCE);
      }
      else
      {
        AddToken(TOKEN_NOT_EQUAL, NULL, NO_PRECEDENCE);
        i++;
      }
    }

    // Parents
    else if (current_char == '(')
    {
      AddToken(TOKEN_LPARENT, NULL, NO_PRECEDENCE);
    }
    else if (current_char == ')')
    {
      AddToken(TOKEN_RPARENT, NULL, NO_PRECEDENCE);
    }

    // Braces
    else if (current_char == '{')
    {
      AddToken(TOKEN_LBRACE, NULL, NO_PRECEDENCE);
    }
    else if (current_char == '}')
    {
      AddToken(TOKEN_RBRACE, NULL, NO_PRECEDENCE);
    }

    else if (current_char == ' ')
    {
      // Ignore
    }

    else
    {
      char msg[64];
      sprintf(msg, "Invalid character 'int: %d'.", current_char);
      error(current_line, SYNTAX_INVALID, msg);
    }
  }

  // PrintTokens(tokens, current_token);  // (Debug)

  result_tokens.result_tokens = tokens;
  result_tokens.result_tokens_count = current_token;

  return result_tokens;
}