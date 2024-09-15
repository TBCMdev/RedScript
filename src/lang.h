#ifndef LANG_H
#define LANG_H

#include <string.h>
#include "token.h"
#include "inb.h"
#include "util.h"

#define INT_TYPE_DEF_ID 0
#define STRING_TYPE_DEF_ID 1
#define FLOAT_TYPE_DEF_ID 2
#define CHAR_TYPE_DEF_ID 3
#define VOID_TYPE_DEF_ID 4

int istypedef(char*);
token_type iskwd(char*, int*);

#endif //LANG_H
