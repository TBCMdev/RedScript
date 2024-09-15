#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>

#include "util.h"
#include "lexer.h"
#include "token.h"
#include "bst.hpp"
#include "nbt.hpp"
#include "mcinbfs.hpp"
#include "commands.hpp"
#include "file.h"


#ifdef RS_FOR_RELEASE
#define REDSCRIPT_PATH "C:/Program Files/Redscript/"
#else 
#define REDSCRIPT_PATH "C:\\Users\\krist\\Desktop\\code\\RedScript\\inb\\lib"
#endif

#define MCMETA(VER) (std::string("{\"pack\":{\"pack_format\": ") + VER + ",\"description\": \"A programming language used for abstracting minecraft's functionalities.\"}}")

#define DATAPACK_FOLDER(W) ([](const std::string &world) { \
    const char* appdata = getenv("APPDATA"); \
    if (!appdata) { \
        throw std::runtime_error("APPDATA environment variable is not set."); \
    } \
    std::cout << appdata << '\n';\
    return std::string(appdata) + "/.minecraft/saves/" + world + "/datapacks/"; }(W))

#define version "48"
#define T_COMPILE_ERROR(_ec, _tokens)                  \
    {                                       \
        __STACK_TRACE = (_tokens)[_At]._Trace; \
        __STACK_TRACE.ec = _ec;             \
        return __STACK_TRACE;               \
    }
#define COMPILE_ERROR(_ec)                  \
    {                                       \
        __STACK_TRACE = tokens[_At]._Trace; \
        __STACK_TRACE.ec = _ec;             \
        return __STACK_TRACE;               \
    }

#define COMPILE_ERROR_RAW                   \
    {                                       \
        int bec = __STACK_TRACE.ec;         \
        __STACK_TRACE = tokens[_At]._Trace; \
        __STACK_TRACE.ec = bec;             \
        return __STACK_TRACE;               \
    }
#define COMPILE_ERROR_P(st, _ec)  \
    {                             \
        *st = tokens[_At]._Trace; \
        st->ec = _ec;             \
    }

#define INT_TYPE_ID 0
#define FLOAT_TYPE_ID 1
#define STRING_TYPE_ID 2
#define VARIABLE_TYPE_ID 3
#define UNKNOWN_TYPE_ID 4
#define UNEVALUATED_NODE_TREE_ID 5
#define CMP_OPERATOR_ID 6

typedef struct tvoidp_t
{
    char *v;
    int t;
} tvoidp;

class shared_tvoidp
{
public:
    std::shared_ptr<void> v;
    int t;

public:
    template <typename _T>
    shared_tvoidp(_T x, int type) : v(std::make_shared<_T>(x)), t(type) {}
};

typedef bst<shared_tvoidp> voidnode;
void _debug_printVoidNodeBst(voidnode &);

namespace mcf
{
    // enum to int
    typedef enum class register_state_t
    {
        FREE,
        OCCUPIED,
        LOCKED
    } register_state;
    typedef enum class register_type_t
    {
        INTEGER_ONLY,
        ALL
    } register_type;
    typedef struct mcf_register_t
    {
        int id;
        std::string display;
        register_type type = register_type::ALL;
        register_state state = register_state::FREE;
        std::string path()
        {
            return RS_PROGRAM_ROOT " registers." + display;
        }
    } mcf_register;
    namespace registers
    {
#define ALL_REGISTER_NAMES "pc", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "rt", "a1", "a2", "a3", "a4", "a5", "a6"
        // general purpose registers

        static inline std::vector<mcf_register> _GPProgramRegisters{
            mcf_register{1, "r1"}, mcf_register{2, "r2"}, mcf_register{3, "r3"}, mcf_register{4, "r4"}, mcf_register{5, "r5"}, mcf_register{6, "r6"}, mcf_register{7, "r7"}};

        inline mcf_register *findFreeGPR()
        {
            for (mcf_register &_reg : _GPProgramRegisters)
                if (_reg.state == register_state::FREE)
                {
                    _reg.state = register_state::OCCUPIED;
                    return &_reg;
                }

            warn("GPR register limit reached. This will be fixed upon release; until now, please lower the amount of nodes in BST.");
            exit(1);
        }
        // program counter (if needed)
        static inline mcf_register pc{0, "pc", register_type::INTEGER_ONLY};

        // return value holder
        static inline mcf_register ret{10, "rt"};
        // arithmatic registers (add more if needed)

        static inline std::vector<mcf_register> _ARProgramRegisters
        {

            mcf_register{11, "a1", register_type::INTEGER_ONLY},
            mcf_register{12, "a2", register_type::INTEGER_ONLY},
            mcf_register{13, "a3", register_type::INTEGER_ONLY},
            mcf_register{14, "a4", register_type::INTEGER_ONLY},
            mcf_register{15, "a5", register_type::INTEGER_ONLY},
        };
        inline mcf_register *findFreeARR()
        {
            for(mcf_register& _reg : _ARProgramRegisters)
            {
                if(_reg.state == register_state::FREE)
                {
                    _reg.state = register_state::OCCUPIED;
                    return &_reg;
                }
            }

            warn("ARR register limit reached. This will be fixed upon release; until now, please lower the amount of nodes in BST.");
            exit(1);
        }
        inline std::pair<mcf_register *, mcf_register *> findTwoOperableRegisters()
        {
            auto *r1 = mcf::registers::findFreeARR();
            auto *r2 = mcf::registers::findFreeARR();
            if (!r1)
                r1 = mcf::registers::findFreeGPR();

            if (!r2)
                r2 = mcf::registers::findFreeGPR();

            return std::pair(r1, r2);
        }   
    }

    typedef enum class rs_instructions_t
    {
        CREATE,
        CALL,
        DELETE,
        EXIT,
        ASSIGN,
        RET,
        CMP,
        CMP_END
    } rs_instructions;
    typedef enum class rs_instructions_pcount_t
    {
        CREATE = 3,
        CALL = 2,
        DELETE = 1,
        EXIT = 0,
        ASSIGN = 2,
        PARAM = 2,
        CMP = 3
    } rs_instructions_pcount;

    typedef struct typedef_info_t
    {
        int _TypeID;
        bool isArray = false;
        int arrayDimension = 0;
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

        inline bool operator==(const rs_variable_t &rhs) const
        {
            return (name == rhs.name);
        }
    } rs_variable;

    typedef struct rs_namespace_t
    {
        std::vector<std::string> descriptors;
        
    } rs_namespace;

    typedef struct function_byte_code_t
    {
        std::string _Name;
        std::vector<byte_code_instruction> _Instructions;
        std::vector<rs_variable> _Variables;
        int _Segments = 1;
        std::vector<rs_variable> _Parameters;
        bool _Inbuilt = false;
    } function_byte_code;

    typedef struct command_t
    {
        std::string name;
        std::string content;
    } command;
    typedef struct mcfunction_t
    {
        std::string path, name;
        std::vector<command> _Instructions;
        std::vector<int> _SegmentBreaks = {};
    } mcfunction;
    typedef struct rscprogram_t
    {
        std::string _WorldName, _FileNameWithoutExtention;
        std::vector<mcfunction> _Functions;
        std::vector<int> _StdDeclEntryIndicies;
        int _ChosenEntryIndex;
    } rscprogram;

    std::filesystem::path createDatapack(const std::string &, const std::string &);
    void fAddCommand(mcfunction &, command &);
    void pAddFunction(rscprogram &, mcfunction &);
    void writemcp(rscprogram &);
    bool createVoidWorld(std::filesystem::path &);
    int findTrailingChar(char, ltoken *, int, int);
    lex_error compileRsc(ltoken *, int, std::shared_ptr<rscprogram> &);
    lex_error buildRsc(rscprogram &);
    voidnode mexpreval(ltoken *, int, int, int, lex_error *, int *);
    std::vector<voidnode> m_evalparams(ltoken *, int, int &, lex_error *);

    std::vector<rs_variable> parseFunctionParameters(ltoken *, int &, std::vector<std::string> &, lex_error *);
    mcfunction fbc_to_mcfunc(function_byte_code &, std::unordered_map<std::string, function_byte_code> &, lex_error *);
}