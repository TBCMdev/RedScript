#include "lexer.h"
#include "file.h"
#include "mcfms.hpp"

#include <filesystem>

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "[ERROR] arguments must be: rscript <path_to_file.rsc> <world_name_to_operate_on>\n";
        return EXIT_FAILURE;
    }
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

    std::shared_ptr<mcf::rscprogram> program;
    lex_error status = mcf::compileRsc(tokens, *tokenCount, program);
    if(status.ec < 0)
    {
        elprint(status, _FileBuffer);

        return status.ec;
    }
    if(!program.get())
    {
        printf("Could not compile program.\n");
        return status.ec;
    }
    program->_FileNameWithoutExtention = std::filesystem::path(argv[1]).stem().string();
    program->_WorldName = argv[2];
    status = mcf::buildRsc(*program);
    
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