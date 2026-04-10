// binary_operations.h
#ifndef BINARYOPS_H
#define BINARYOPS_H

#include <stddef.h>

#include "../definitions.h"

Token BinaryOperation(Token *tokens, ssize_t current_function, size_t line_num);

Token ParseBinaryOperation(Token *tokens, size_t tokens_count, ssize_t current_function,
                           size_t line_num);

#endif