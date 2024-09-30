#ifndef __PARSER_H
#define __PARSER_H

#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

typedef enum {
    TOKEN_STRING,
    TOKEN_ASSIGN,
    TOKEN_VAR,
    TOKEN_PIPE,
    TOKEN_REDIR,
} token_type_t;

typedef struct {
    token_type_t type;
    char* value;
} token_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static const char *TOKEN_TO_STRING[] = {
    "TOKEN STRING", "TOKEN ASSIGN", "TOKEN VAR", "TOKEN PIPE", "TOKEN REDIR",
};
#pragma GCC diagnostic pop

token_t** tokenize(const char* inputbuffer, size_t bufferlen, int* numtokens);

#endif
