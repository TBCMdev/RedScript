#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include "token.h"
#include "util.h"

#define MCMETA(VER) (std::string("{\"pack\":{\"pack_format\": ") + VER + ",\"description\": \"A programming language used for abstracting minecraft's functionalities.\"}}")
#define DATAPACK_FOLDER(W) (std::string(getenv("APPDATA")) + "/Roaming/.minecraft/saves/" + W + "/datapacks/")
#define version "48"

#define COMPILE_ERROR(_ec) {__STACK_TRACE.ec = _ec;__STACK_TRACE.at = _At;  return __STACK_TRACE;}


namespace mcf
{
    typedef struct byte_code_instruction
    {
        int _id;
        std::vector<std::string> _Parameters;
    };
    typedef struct function_byte_code
    {
        std::vector<byte_code_instruction> _Instructions;
    };
    typedef struct command
    {
        std::string name;
        std::vector<std::string> parameters; 
    };
    typedef struct mcfunction
    {
        std::string path, name;
        std::vector<command> _Instructions;
    };
    typedef struct rscprogram
    {
        std::string _WorldName, _FileNameWithoutExtention;
        std::vector<mcfunction> _Functions;
        std::vector<int> _StdDeclEntryIndex;
        int _ChosenEntryIndex;
    };

    std::filesystem::path createDatapack(const std::string&, const std::string&);
    void fAddCommand(mcfunction&, command&);
    void pAddFunction(rscprogram&, mcfunction&);
    void writemcp(rscprogram&);
    lex_error compileRsc(ltoken*, int, rscprogram*);
    lex_error buildRsc(rscprogram&);
}