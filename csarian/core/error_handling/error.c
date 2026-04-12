// error.c
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

int error(int line, int error_type, char *error_message)
{
  switch (error_type)
  {
    case MEM_MALLOC_FAILED:
      fprintf(stderr, "\nMEM_MALLOC_FAILED Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case MEM_CALLOC_FAILED:
      fprintf(stderr, "\nMEM_CALLOC_FAILED Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case MEM_REALLOC_FAILED:
      fprintf(stderr, "\nMEM_REALLOC_FAILED Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case SYNTAX_INCOMPLETE_EXPRESSION:
      fprintf(stderr, "\nSYNTAX_INCOMPLETE_EXPRESSION Error (%d), at line: %d\n%s\n", error_type,
              line, error_message);
      exit(1);
    case SYNTAX_INCOMPLETE_PARENT:
      fprintf(stderr, "\nSYNTAX_INCOMPLETE_PARENT Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case TOKENERR_UNKNOWN_TOKEN:
      fprintf(stderr, "\nTOKENERR_UNKNOWN_TOKEN Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case TYPE_INVALID:
      fprintf(stderr, "\nTYPE_INVALID Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case IDENTIFIER_UNKNOWN:
      fprintf(stderr, "\nIDENTIFIER_UNKNOWN Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case TYPE_INVALID_OPERANDS:
      fprintf(stderr, "\nTYPE_INVALID_OPERANDS Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case SYNTAX_INVALID:
      fprintf(stderr, "\nSYNTAX_INVALID Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case SYNTAX_INCOMPLETE_BRACE:
      fprintf(stderr, "\nSYNTAX_INCOMPLETE_BRACE Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case TYPE_INVALID_OPERATOR:
      fprintf(stderr, "\nTYPE_INVALID_OPERATOR Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case SYNTAX_ILLEGAL_FUNCTION:
      fprintf(stderr, "\nSYNTAX_ILLEGAL_FUNCTION Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case INTERNAL_ERROR:
      fprintf(stderr, "\nINTERNAL_ERROR Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);
    case TYPE_INVALID_ARGUMENTS:
      fprintf(stderr, "\nTYPE_INVALID_ARGUMENTS Error (%d), at line: %d\n%s\n", error_type, line,
              error_message);
      exit(1);

    default:
      fprintf(stderr, "\n[!] Unknown error type: %d\n", error_type);
      exit(1);
  }

  return 0;
}