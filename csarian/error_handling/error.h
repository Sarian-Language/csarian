// error.h
#ifndef ERROR_H
#define ERROR_H

// Error Types
typedef enum
{
    // --- Memory errors (MEM): ---
    MEM_MALLOC_FAILED,
    MEM_CALLOC_FAILED,
    MEM_REALLOC_FAILED,

    // --- Syntax errors (SYNTAX): ---
    SYNTAX_INVALID,
    SYNTAX_INCOMPLETE_EXPRESSION,
    SYNTAX_INCOMPLETE_PARENT,

    // --- Token errors (TOKENERR): ---
    TOKENERR_UNKNOWN_TOKEN,

    // --- Identifiers errors (IDENTIFIER): ---
    IDENTIFIER_UNKNOWN,

    // --- Type errors (TYPE): ---
    TYPE_INVALID,
    TYPE_INVALID_OPERANDS
} ErrorType;

int error(int line, int error_type, char *error_message);

#endif