// interpreter.c
#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"

int interpreter(Token *tokens, int numTokens)
{
    for (int i = 0; i < numTokens; i++)
    {
        // This was just a temporal code for testing that everything works, it will be changed.
        if (tokens[i].type == TOKEN_NUMERIC)
        {
            if (tokens[i+1].type == TOKEN_PLUS)
            {
                if (tokens[i+2].type == TOKEN_NUMERIC)
                {
                    int a = atoi(tokens[i].value);
                    int b = atoi(tokens[i+2].value);

                    printf("%d",a+b);
                }
            }
        }
    }
}