// main.c
#include <stdio.h>
#include <stdlib.h>

#include "csarian/core/interpreter/interpreter.h"
#include "csarian/core/lexer/lexer.h"
#include "csarian/utils/token_utils/token_utils.h"
#include "main.h"

ResultTokens main_tokens;

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "rb");
  if (!file)
  {
    perror("[Main] Error opening file");
    return 1;
  }

  if (fseek(file, 0, SEEK_END) != 0)
  {
    perror("[Main] Error seeking file end");
    fclose(file);
    return 1;
  }

  long filesize = ftell(file);
  if (filesize < 0)
  {
    perror("[Main] Error getting file size");
    fclose(file);
    return 1;
  }
  fseek(file, 0, SEEK_SET);

  char *code = malloc((size_t)filesize + 1);
  if (!code)
  {
    perror("[Main] Error: insufficient memory");
    fclose(file);
    return 1;
  }

  size_t bytes_read = fread(code, 1, (size_t)filesize, file);
  if (bytes_read != (size_t)filesize)
  {
    perror("[Main] Error reading file completely");
    free(code);
    fclose(file);
    return 1;
  }

  code[filesize] = '\0';
  fclose(file);

  main_tokens = Lexer(code);
  Interpreter(main_tokens.result_tokens, main_tokens.result_tokens_count, false, -1, 1, -1, -1, 0,
              true);

  free(code);
  return 0;
}