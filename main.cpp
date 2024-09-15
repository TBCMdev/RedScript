

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

    if(!*_FileBuffer)
        return -1;

    std::string sFileBuffer = _FileBuffer;
    ltoken* tokens = (ltoken*) malloc(sizeof(ltoken) * INITIAL_GEN_CAPACITY);
    int* tokenCount = (int*) malloc(sizeof(int));
    lex_error _Status = _lex(&tokens, sFileBuffer.data(), sFileBuffer.length(), tokenCount);


    if(_Status.ec < 0)
    {
        elprint(_Status, sFileBuffer.data());

        return _Status.ec;
    }

    std::string mainFileDirectory = std::filesystem::absolute(argv[1]).parent_path().string();

    _Status = _preprocess(&tokens, *tokenCount, mainFileDirectory, sFileBuffer, tokenCount); 

    if(_Status.ec < 0)
    {
        elprint(_Status, sFileBuffer.data());

        return _Status.ec;
    }

    if(tokens == NULL) 
    {
        printf("Empty File.\n");
        return EXIT_FAILURE;
    }



    std::shared_ptr<mcf::rscprogram> program;
    _Status = mcf::compileRsc(tokens, *tokenCount, program);
    if(_Status.ec < 0)
    {
        elprint(_Status, sFileBuffer.data());

        return _Status.ec;
    }
    if(!program.get())
    {
        printf("Could not compile program.\n");
        return _Status.ec;
    }
    program->_FileNameWithoutExtention = std::filesystem::path(argv[1]).stem().string();
    program->_WorldName = argv[2];
    _Status = mcf::buildRsc(*program);
    
    if(_Status.ec < 0)
    {
        elprint(_Status, sFileBuffer.data());

        return _Status.ec;
    }
    for(int i = 0; i < *tokenCount; i++)
    {
        ltoken* t = &tokens[i];
        if(t->_Repr != nullptr)
        {
            tprint(t);
            printf("\n");
        }
    }

    std::cout << "file(s) compiled successfully.\n";

    free(tokenCount);
    free(tokens);
}