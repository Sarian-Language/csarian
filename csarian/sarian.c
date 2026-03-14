// sarian.c
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "definitions.h"

int sarian(char *code)
{
    for (int i = 0; i < strlen(code); i++)
    {
        char line[8096]; // Make the line size dynamic
        char current_char = code[i];

        if (current_char == '\n' || current_char == '\0')
        {
            lexer(line);

            line[0] = '\0';
        }

        else
        {
            // Add current character to the line.
            size_t len = strlen(line);
            if (len < sizeof(line) - 1) {
                line[len] = current_char;
                line[len + 1] = '\0';
            }
        }
    }
}