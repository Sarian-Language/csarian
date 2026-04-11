// definitions.h
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// Variable types
typedef enum
{
  INTEGER,
  FLOAT,
  STRING,
  INVALID
} VariableType;

// Variable struct
typedef struct
{
  char *key;
  VariableType type;
  void *value;
} Variable;

// Token types
typedef enum
{
  TOKEN_EOF,  // End Of File
  TOKEN_EOL,  // End Of Line
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_ASTERISK,
  TOKEN_SLASH,
  TOKEN_PERCENT,
  TOKEN_INT_LITERAL,
  TOKEN_FLOAT_LITERAL,
  TOKEN_ASSIGNMENT,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_LPARENT,
  TOKEN_RPARENT,
  TOKEN_IF,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_ELSE,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_EQUAL,
  TOKEN_NOT_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_EXCLAMATION,
  TOKEN_COLON,
  TOKEN_COMMA,
  TOKEN_DBG_PRINT,
  TOKEN_FN,
  TOKEN_IMPORT,
  TOKEN_RETURN,
  TOKEN_GOTO,
  TOKEN_NULL
} TokenType;

// Token precedences
typedef enum
{
  NO_PRECEDENCE = 0,

  PLUS_PRECEDENCE = 10,
  MINUS_PRECEDENCE = 10,

  ASTERISK_PRECEDENCE = 20,
  SLASH_PRECEDENCE = 20,
  PERCENT_PRECEDENCE = 20
} TokenPrecedence;

// Token struct
typedef struct
{
  TokenType type;
  char *value;
  TokenPrecedence precedence;
} Token;

// Function struct
typedef struct
{
  char *key;

  Variable *function_variables;
  size_t variables_count;
  size_t variables_size;

  size_t start;
  size_t end;
} Function;

// Labels (goto)
typedef struct
{
  char *key;
  size_t start;
} Label;

// Macros
#define IS_BINARY_OPERATOR(t)                                                                \
  ((t) == TOKEN_PLUS || (t) == TOKEN_MINUS || (t) == TOKEN_ASTERISK || (t) == TOKEN_SLASH || \
   (t) == TOKEN_PERCENT)

#define IS_VALID_BINARY_OPERAND(t)                                                  \
  ((t) == TOKEN_INT_LITERAL || (t) == TOKEN_FLOAT_LITERAL || (t) == TOKEN_STRING || \
   (t) == TOKEN_IDENTIFIER)

#define IS_COMPARISON_TOKEN(t)                                             \
  ((t) == TOKEN_EQUAL || (t) == TOKEN_NOT_EQUAL || (t) == TOKEN_GREATER || \
   (t) == TOKEN_GREATER_EQUAL || (t) == TOKEN_LESS || (t) == TOKEN_LESS_EQUAL)

#define I_PREVIOUS_TOKEN tokens[i - 1]
#define I_CURRENT_TOKEN tokens[i]
#define I_NEXT_TOKEN_1 tokens[i + 1]
#define I_NEXT_TOKEN_2 tokens[i + 2]
#define I_NEXT_TOKEN_3 tokens[i + 3]

#define PTR_I_PREVIOUS_TOKEN tokens[*i - 1]
#define PTR_I_CURRENT_TOKEN tokens[*i]
#define PTR_I_NEXT_TOKEN_1 tokens[*i + 1]
#define PTR_I_NEXT_TOKEN_2 tokens[*i + 2]
#define PTR_I_NEXT_TOKEN_3 tokens[*i + 3]

#define J_PREVIOUS_TOKEN tokens[j - 1]
#define J_CURRENT_TOKEN tokens[j]
#define J_NEXT_TOKEN_1 tokens[j + 1]
#define J_NEXT_TOKEN_2 tokens[j + 2]
#define J_NEXT_TOKEN_3 tokens[j + 3]

#endif