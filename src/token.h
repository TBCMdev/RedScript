#ifndef TOKEN_H
#define TOKEN_H

#include "error.h"

typedef enum token_type_t
{
    INTEGER_LITERAL = 0,
    FLOAT_LITERAL = 1,
    STRING_LITERAL = 2,
    TYPE_DEF,
    WORD,
    
    VARIABLE_DEF,

    FUNC_DEF,
    INB_FUNC_DEF,
    OPERATOR,
    COMPARISON_OPERATOR,
    CHARACTER,
    OPEN_BR,
    OPEN_SQBR,
    OPEN_CBR,
    CLOSED_BR,
    CLOSED_CBR,
    CLOSED_SQBR,
    NEGATOR,
    FILE_INCLUDE,
    NEWLINE,
    SEMI_COLON,
    COMMA,
    COLON,
    ASSIGN,
    HASHTAG,
    SELECTOR,
    UNKNOWN,
} token_type;

typedef struct ltoken_t
{
    char *_Repr;
    token_type _Type;
    int _Info;
    lex_error _Trace;
} ltoken;

#endif // TOKEN_H
