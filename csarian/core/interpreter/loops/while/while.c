// while.c
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/definitions.h"
#include "csarian/runtime/functions/fn.h"
#include "csarian/utils/debug/debug.h"

While *while_loops;
size_t while_loops_count;

size_t while_loops_size;

void InitWhileLoops()
{
  while_loops_count = 0;
  while_loops_size = 16;

  while_loops = calloc(while_loops_size, sizeof(While) * while_loops_size);
}

void AddWhileLoop(size_t while_start, size_t while_end, Token *comparison_tokens,
                  size_t comparison_tokens_count)
{
  if (while_loops_count >= while_loops_size)
  {
    size_t new_size = while_loops_size * 2;

    While *tmp = realloc(while_loops, sizeof(While) * new_size);

    while_loops = tmp;

    while_loops_size *= 2;
  }

  if (while_loops[while_loops_count].while_comparison_tokens.result_tokens == NULL)
  {
    while_loops[while_loops_count].while_comparison_tokens.result_tokens =
      calloc(comparison_tokens_count, sizeof(Token));
  }

  while_loops[while_loops_count].while_block_start = while_start;
  while_loops[while_loops_count].while_block_end = while_end;

  while_loops[while_loops_count].while_comparison_tokens.result_tokens_count =
    comparison_tokens_count;

  memcpy(while_loops[while_loops_count].while_comparison_tokens.result_tokens, comparison_tokens,
         sizeof(Token) * comparison_tokens_count);

  while_loops_count++;
}
