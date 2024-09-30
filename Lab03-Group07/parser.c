#include <assert.h>
#include "parser.h"

// This parser is provided for your convenience as a reference. It will work as is.
// You are not required to create a parser more complex than this one.
// It transforms a string into a list of tokens. Each token is a string that represents a word or a special character.
// The parser also support quoting and variables.

token_t** tokenize(const char* inputbuffer, size_t bufferlen, int* numtokens) {
    int in_string = FALSE;
    char terminator = '\0';
    size_t start_string = 0;
    size_t end_string = 0;

    token_t** tokens = NULL;
    *numtokens = 0;

    size_t bufpos = 0;
    while ( bufpos < strlen(inputbuffer) ) {
        if ( in_string ) {
            if ( inputbuffer[bufpos] == terminator || inputbuffer[bufpos] == '\n' ) {
                tokens = realloc(tokens, (*numtokens+1)*sizeof(token_t*));
                tokens[*numtokens] = malloc(sizeof(token_t));
                if (inputbuffer[start_string] == '$') {
                    tokens[*numtokens]->type = TOKEN_VAR;
                    tokens[*numtokens]->value = malloc(end_string-start_string);
                    strncpy(tokens[*numtokens]->value, inputbuffer+start_string+1, end_string-start_string);
                    tokens[*numtokens]->value[end_string-start_string] = 0;
                } else {
                    if ( terminator == '"' ) {
                        tokens[*numtokens]->type = TOKEN_STRING;
                        tokens[*numtokens]->value = malloc(end_string-start_string);
                        strncpy(tokens[*numtokens]->value, inputbuffer+start_string+1, end_string-start_string+1);
                        tokens[*numtokens]->value[end_string-start_string] = 0;
                    }
                    else {
                        tokens[*numtokens]->type = TOKEN_STRING;
                        tokens[*numtokens]->value = malloc(end_string-start_string+1);
                        strncpy(tokens[*numtokens]->value, inputbuffer+start_string, end_string-start_string+1);
                        tokens[*numtokens]->value[end_string-start_string+1] = 0;
                    }
                }
                (*numtokens)++;
                in_string = FALSE;
            } else {
                end_string++;
            }
        } else {
            if ( inputbuffer[bufpos] == ' ' || inputbuffer[bufpos] == '\n' ) {
                //Do nothing
            } else if ( inputbuffer[bufpos] == '=' ) {
                tokens = realloc(tokens, (*numtokens+1)*sizeof(token_t*));
                tokens[*numtokens] = malloc(sizeof(token_t));
                tokens[*numtokens]->type = TOKEN_ASSIGN;
                tokens[*numtokens]->value = NULL;
                (*numtokens)++;
            } else if ( inputbuffer[bufpos] == '|' ) {
                tokens = realloc(tokens, (*numtokens+1)*sizeof(token_t*));
                tokens[*numtokens] = malloc(sizeof(token_t));
                tokens[*numtokens]->type = TOKEN_PIPE;
                tokens[*numtokens]->value = NULL;
                (*numtokens)++;
            } else if ( inputbuffer[bufpos] == '>' ) {
                tokens = realloc(tokens, (*numtokens+1)*sizeof(token_t*));
                tokens[*numtokens] = malloc(sizeof(token_t));
                tokens[*numtokens]->type = TOKEN_REDIR;
                tokens[*numtokens]->value = NULL;
                (*numtokens)++;
            } else {
                in_string = TRUE;
                start_string = bufpos;
                end_string = bufpos;
                if ( inputbuffer[bufpos] == '"' ) {
                    terminator = '"';
                } else {
                    terminator = ' ';
                }
            }
        }
        bufpos++;
    }
    // Check whether we were in a string when we hit the end of the buffer
    if ( in_string ) {
        tokens = realloc(tokens, (*numtokens+1)*sizeof(token_t*));
        tokens[*numtokens] = malloc(sizeof(token_t));
        if (inputbuffer[start_string] == '$') {
            tokens[*numtokens]->type = TOKEN_VAR;
            tokens[*numtokens]->value = malloc(end_string-start_string);
            strncpy(tokens[*numtokens]->value, inputbuffer+start_string+1, end_string-start_string);
            tokens[*numtokens]->value[end_string-start_string] = 0;
        } else {
            tokens[*numtokens]->type = TOKEN_STRING;
            tokens[*numtokens]->value = malloc(end_string-start_string+1);
            strncpy(tokens[*numtokens]->value, inputbuffer+start_string, end_string-start_string+1);
            tokens[*numtokens]->value[end_string-start_string+1] = 0;
        }
        (*numtokens)++;
        in_string = FALSE;
    }

    return tokens;    
}
