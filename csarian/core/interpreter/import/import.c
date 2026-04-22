// import.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csarian/core/error_handling/error.h"
#include "csarian/core/lexer/lexer.h"
#include "csarian/definitions.h"
#include "csarian/utils/token_utils/token_utils.h"

void Import(Token *tokens, size_t *tokens_count, size_t *i, size_t line_num)
{
  if (PTR_I_NEXT_TOKEN_1.type == TOKEN_STRING)
  {
    FILE *file = fopen(PTR_I_NEXT_TOKEN_1.value, "rb");
    if (!file)
    {
      perror("[Interpreter] Error opening import file");
      exit(1);
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
      perror("[Interpreter] Error seeking import file end");
      fclose(file);
      exit(1);
    }

    long filesize = ftell(file);
    if (filesize < 0)
    {
      perror("[Interpreter] Error getting import file size");
      fclose(file);
      exit(1);
    }
    fseek(file, 0, SEEK_SET);

    char *code = malloc((size_t)filesize + 1);
    if (!code)
    {
      fclose(file);
      error(line_num, MEM_MALLOC_FAILED, "Failed to malloc() code.");
    }

    size_t bytes_read = fread(code, 1, (size_t)filesize, file);
    if (bytes_read != (size_t)filesize)
    {
      perror("[Interpreter] Error reading file completely");
      free(code);
      fclose(file);
      exit(1);
    }

    code[filesize] = '\0';
    fclose(file);

    ResultTokens import_tokens = Lexer(code);

    size_t insert_pos = *i;
    size_t remove_count = 2;

    // Determine how many tokens to insert (excluding TOKEN_EOF)
    size_t tokens_to_insert = import_tokens.result_tokens_count;
    if (tokens_to_insert > 0 && import_tokens.result_tokens[tokens_to_insert - 1].type == TOKEN_EOF)
    {
      tokens_to_insert--;
    }

    // Resize the tokens array
    *tokens_count = *tokens_count - remove_count + tokens_to_insert;
    tokens = realloc(tokens, sizeof(Token) * *tokens_count);
    if (!tokens)
    {
      free(code);
      error(line_num, MEM_REALLOC_FAILED, "Failed to realloc tokens array");
    }

    // Move the subsequent tokens forward
    memmove(&tokens[insert_pos + tokens_to_insert], &tokens[insert_pos + remove_count],
            sizeof(Token) * (*tokens_count - insert_pos - tokens_to_insert));

    // Copy imported tokens
    for (size_t j = 0; j < tokens_to_insert; j++)
    {
      tokens[insert_pos + j] = import_tokens.result_tokens[j];
    }

    free(code);

    *i = insert_pos - 1;
  }
}