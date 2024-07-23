#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "error.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_GEN_CAPACITY 256

#define IS_OP(x) (x == '!' || x == '+' || x == '-' || x == '/' || x == '*' || x == '%')

#define LEX_IFFAIL(mallocd) if (mallocd == NULL) {free(mallocd);__STACK_TRACE.ec = MEMORY_ERROR; return __STACK_TRACE;}
#define LEX_ERROR(_ec) {__STACK_TRACE.ec = _ec;__STACK_TRACE.at = i;  return __STACK_TRACE;}

#define tprint(t) printf("{type:%d, repr:%s, info:%d}", t->_Type, t->_Repr, t->_Info);


lex_error _lex(ltoken*, char*, int, int*);
// util
int isfloat(char*);
int isint(char*);
#endif //LEXER_H
