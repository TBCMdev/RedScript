#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "token.h"
#include "util.h"
#include "bst.hpp"
#include "lexer.h"
#include "nbt.hpp"

#define MCMETA(VER) (std::string("{\"pack\":{\"pack_format\": ") + VER + ",\"description\": \"A programming language used for abstracting minecraft's functionalities.\"}}")

#define DATAPACK_FOLDER(W) ([](const std::string& world) { \
    const char* appdata = getenv("APPDATA"); \
    if (!appdata) { \
        throw std::runtime_error("APPDATA environment variable is not set."); \
    } \
    std::cout << appdata << '\n';\
    return std::string(appdata) + "/.minecraft/saves/" + world + "/datapacks/"; \
}(W))

#define version "48"

#define COMPILE_ERROR(_ec) {__STACK_TRACE = tokens[_At]._Trace;__STACK_TRACE.ec = _ec; return __STACK_TRACE;}
#define COMPILE_ERROR_P(st, _ec) {*st = tokens[_At]._Trace;st->ec = _ec;}

#define INT_TYPE_ID 0
#define FLOAT_TYPE_ID 1
#define STRING_TYPE_ID 2
#define UNKNOWN_TYPE_ID 3
#define UNEVALUATED_NODE_TREE_ID 4
typedef struct tvoidp_t
{
    char* v;
    int   t;
} tvoidp;

class shared_tvoidp
{
    std::shared_ptr<void> v;
    int t;

public:
    template<typename _T>
    shared_tvoidp(_T x, int type) : v(std::make_shared<_T>(x)), t(type) {}
};


typedef bst<tvoidp> voidnode;
void _debug_printVoidNodeBst(voidnode&);

namespace mcf
{
// enum to int
    typedef struct mcf_register_t
    {
        int id;
        const char* display;
    } mcf_register;

    // general purpose registers
    static inline mcf_register r1{1, "r1"};
    static inline mcf_register r2{2, "r2"};
    static inline mcf_register r3{3, "r3"};
    static inline mcf_register r4{4, "r4"};
    static inline mcf_register r5{5, "r5"};
    static inline mcf_register r6{6, "r6"};
    static inline mcf_register r7{7, "r7"};
    static inline mcf_register r8{8, "r8"};
    static inline mcf_register r9{9, "r9"};
    // program counter (if needed)
    static inline mcf_register pc{0, "pc"};

    // return value holder
    static inline mcf_register ret{10, "rt"};

    // arithmatic registers (add more if needed)
    static inline mcf_register a1{11, "a1"};
    static inline mcf_register a2{12, "a2"};
    static inline mcf_register a3{13, "a3"};
    static inline mcf_register a4{14, "a4"};
    static inline mcf_register a5{15, "a5"};



    typedef enum class rs_instructions_t
    {
        CREATE, CALL, DELETE, EXIT, ASSIGN, PARAM
    } rs_instructions;
    typedef enum class rs_instructions_pcount_t
    {
        CREATE = 3, CALL = 2, DELETE = 1, EXIT = 0, ASSIGN = 2, PARAM = 2
    } rs_instructions_pcount;
    

    typedef struct typedef_info_t
    {
        int _TypeID;
        bool isArray = false;
        int  arrayDimension = 0;
        bool isTemplate = false;
        int _TemplateID = -1;

    } typedef_info;

    typedef struct byte_code_instruction_t
    {
        rs_instructions _id;
        std::vector<shared_tvoidp> _Parameters;
    } byte_code_instruction;
    // CREATE -> 3 parameters: name<string>, type<int>, arrayDimension<int>

    typedef struct rs_variable_t
    {
        std::string name;
        typedef_info type;



    } rs_variable;
    typedef struct function_byte_code_t
    {
        std::string _Name;
        std::vector<byte_code_instruction> _Instructions;
        std::vector<rs_variable> _Variables;
    } function_byte_code;

    typedef struct command_t
    {
        std::string name;
        std::vector<std::string> parameters; 
    } command;
    typedef struct mcfunction_t
    {
        std::string path, name;
        std::vector<command> _Instructions;
    } mcfunction;
    typedef struct rscprogram_t
    {
        std::string _WorldName, _FileNameWithoutExtention;
        std::vector<mcfunction> _Functions;
        std::vector<int> _StdDeclEntryIndicies;
        int _ChosenEntryIndex;
    } rscprogram;

    std::filesystem::path createDatapack(const std::string&, const std::string&);
    void fAddCommand(mcfunction&, command&);
    void pAddFunction(rscprogram&, mcfunction&);
    void writemcp(rscprogram&);
    bool createVoidWorld(std::filesystem::path&);
    int findTrailingChar(char, ltoken*, int, int);
    lex_error compileRsc(ltoken*, int, rscprogram*);
    lex_error buildRsc(rscprogram&);


    voidnode mexpreval(ltoken*, int, int, int, lex_error*, int*);
    std::vector<voidnode> m_evalparams(ltoken*, int, int&, lex_error*);

    std::vector<rs_variable> parseFunctionParameters(ltoken*, int&,std::vector<std::string>&, lex_error*);

}