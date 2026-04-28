// binary_operations.h
#ifndef BINARYOPS_H
#define BINARYOPS_H

#include <stddef.h>

#include "csarian/definitions.h"

Token BinaryOperation(Token *tokens, size_t tokens_count, ssize_t current_function,
                      size_t line_num);

#endif