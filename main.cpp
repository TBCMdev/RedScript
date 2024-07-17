#include "lexer.h"
#include "file.h"
#include "mcf.hpp"

int main(int argc, char** argv)
{
    char* _FileBuffer;

    sfread(argv[1], &_FileBuffer);

    int _FileBufferLen = strlen(_FileBuffer);

    ltoken* tokens = (ltoken*) malloc(sizeof(ltoken) * INITIAL_GEN_CAPACITY);

    int* tokenCount = (int*) malloc(sizeof(int));

    lex_error _Status = _lex(tokens, _FileBuffer, _FileBufferLen, tokenCount);

    if(_Status.ec < 0)
    {
        elprint(_Status, _FileBuffer);

        return _Status.ec;
    }
    if(tokens == NULL) 
    {
        printf("Empty File.\n");
        return EXIT_FAILURE;
    }

    mcf::rscprogram program = mcf::compileRsc(tokens);

    lex_error status = mcf::buildRsc(program);
    if(status.ec < 0)
    {
        elprint(status, _FileBuffer);

        return status.ec;
    }
    for(int i = 0; i < *tokenCount; i++)
    {
        ltoken* t = &tokens[i];
        if(t->_Repr != NULL)
        {
            tprint(t);
            printf("\n");
            free(t->_Repr);
        }
    }

    free(tokenCount);
    free(tokens);

}