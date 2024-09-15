#include "lang.h"

int istypedef(char* c)
{
    if(seq(c, "float")) return FLOAT_TYPE_DEF_ID;
    if(seq(c, "int")) return INT_TYPE_DEF_ID;
    if(seq(c, "string")) return STRING_TYPE_DEF_ID;
    if(seq(c, "unknown")) return VOID_TYPE_DEF_ID;
    // if(seq(c, "char")) return CHAR_TYPE_DEF_ID;

    return -1;

}
token_type iskwd(char* c, int* info)
{
    int t = istypedef(c);
    if(t > 0) return TYPE_DEF;

    if(seq(c, "use")) return FILE_INCLUDE;
    return UNKNOWN;
}