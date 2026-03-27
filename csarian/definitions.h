// definitions.h
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdbool.h>
#include <stddef.h>

typedef enum
{
  INT,
  FLOAT,
  STRING,
  INVALID
} VariableType;

// Global variable
typedef struct
{
    char *key;
    VariableType type;
    void* value;
} GlobalVariable;

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
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
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
    TOKEN_DBG_PRINT,
    TOKEN_FN,
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
  
  size_t start;
  size_t end;
} Function;

// Macros
#define IS_BINARY_OPERATOR(t) ((t) == TOKEN_PLUS || (t) == TOKEN_MINUS || (t) == TOKEN_ASTERISK || \
                               (t) == TOKEN_SLASH || (t) == TOKEN_PERCENT)

#define IS_VALID_BINARY_OPERAND(t) ((t) == TOKEN_INT_LITERAL || (t) == TOKEN_FLOAT_LITERAL || (t) == TOKEN_STRING || \
                                    (t) == TOKEN_IDENTIFIER)

#endif