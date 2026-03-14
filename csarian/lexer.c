// lexer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "definitions.h"
#include "interpreter.h"

#define MAX_TOKENS 5000

// Debug feature
void printTokens(Token *tokens, int numTokens) {
    for (int i = 0; i < numTokens; i++) {
        switch (tokens[i].type) {
            case TOKEN_PLUS:
                printf("[PLUS] ");
                break;
            case TOKEN_NUMERIC:
                printf("[NUMERIC (%s)] ",tokens[i].value);
                break;
            default:
                printf("[UNKNOWN] ");
        }
    }
    printf("\n");
}

int lexer(char *line)
{
    Token tokens[MAX_TOKENS];
    int current_token = 0;

    for (int i = 0; i < strlen(line); i++)
    {
        char current_char = line[i];

        if (current_char == '+')
        {
            tokens[current_token] = (Token){TOKEN_PLUS};
            // printf("Adding plus token\n");
            current_token++;
        }

        if (isdigit(current_char))
        {
            char number[strlen(line)];
            number[0] = current_char;
            number[1] = '\0';

            for (int j = i; j < strlen(line); j++)
            {
                if (isdigit(line[j+1]))
                {
                    int len = strlen(number);
                    number[len] = line[j+1];
                    number[len + 1] = '\0';
                    i++;
                }
                else
                {
                    // printf("Adding numeric token\n");
                    tokens[current_token] = (Token){TOKEN_NUMERIC, strdup(number)};
                    break;
                }
            }
            
            //printf("%s",number);
            current_token++;
        }
    }

    // printTokens(tokens, current_token);

    interpreter(tokens, current_token);
}