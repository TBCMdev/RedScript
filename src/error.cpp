#include "error.h"

#include <string.h>
#include <stdlib.h>

void errstr(int ec, char* out, int s)
{
    switch(ec)
    {
    case STRING_LITERAL_NOT_CLOSED:
        strncpy(out, "String Literal Not Closed", s);
        break;
    case MEMORY_ERROR:
        strncpy(out, "Memory Allocation", s);
        break;
    case INVALID_NUMERICAL_NOTATION:
        strncpy(out, "Invalid Numerical Notation", s);
        break;
    default:
        strncpy(out, "Unknown", s);
        break;
    }
    out[s - 1] = '\0';
}

void elprint(lex_error error, char* unlexed_c)
{
    int len = strlen(unlexed_c);

    int at = error.at;
    
    while (--at > 0 && unlexed_c[at] != '\n');
    int lineStart = at;
    while (++at < len && unlexed_c[at] != '\n');
    int lineEnd = at;

    int lineLength = lineEnd - lineStart;
    char* line = (char*)malloc(lineLength * sizeof(char) + 1);

    memcpy(line, &unlexed_c[lineStart], lineLength);
    line[lineLength] = '\0';
    if(line == NULL)
    {
        printf("Unexpected error occured.");
        return;
    }
    char errcn[32] = {0};
    errstr(error.ec, errcn, 32);
    printf("\n%s Error(em-%d):\n", errcn, abs(error.ec));
    printf("\t%s\n", line); 
    printf("\t%*s^ HERE\n\n", error.cpos, "");
}