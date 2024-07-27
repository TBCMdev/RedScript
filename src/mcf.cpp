#include "mcf.hpp"

void create_level_dat(const std::string &path)
{
    nbt::tags::compound_tag root("Data");

    // Add various tags required for level.dat
    root.put("version", 19133); // todo change for different versions
    root.put("DataVersion", 2230);
    root.put("LevelName", std::string("New Void World"));
    root.put("Dimension", 0);
    root.put("Difficulty", 2);
    root.put("GameType", 1); // Creative mode
    root.put("LastPlayed", 0);
    root.put("Time", 0);
    root.put("DayTime", 0);
    root.put("clearWeatherTime", 0);
    root.put("rainTime", 0);
    root.put("raining", 0);
    root.put("thunderTime", 0);
    root.put("thundering", 0);
    // Write the NBT data to level.dat file
    std::ofstream ofs(path, std::ios::binary);
    root.write(ofs);
}
bool mcf::createVoidWorld(std::filesystem::path &p)
{
    try
    {
        std::string worldName = p.stem().string();
        std::filesystem::create_directory(p); // create world folder
        std::filesystem::create_directories(p / "/region");
        std::filesystem::create_directories(p / "/data");
        std::filesystem::create_directories(p / "/playerdata");

        // Create a minimal level.dat file
        create_level_dat((p / "/level.dat").string());
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

std::filesystem::path mcf::createDatapack(const std::string &world, const std::string &name)
{
    std::string dpf = DATAPACK_FOLDER(world);
    std::filesystem::path worldPath = std::filesystem::path(dpf).parent_path();
    if (!std::filesystem::exists(worldPath)) // if the world doesnt exist
    {
        std::cout << "[INFO] World " << world << " does not exist. Creating...\n";
        if (!mcf::createVoidWorld(worldPath))
        {
            std::cout << "[ERROR] Could not create world. Aborting...\n";
            return std::filesystem::path();
        }
    }
    std::filesystem::path path = dpf + name + "/pack.mcmeta";
    std::filesystem::create_directory(path.parent_path());

    std::ofstream ofs(path);
    ofs << MCMETA(version);
    ofs.close();

    path /= "functions";
    std::filesystem::create_directory(path);

    return path;
}
void mcf::fAddCommand(mcfunction &f, command &c)
{
    f._Instructions.push_back(c);
}
void mcf::pAddFunction(rscprogram &pro, mcfunction &f)
{
    pro._Functions.push_back(f);
}
void mcf::writemcp(rscprogram &pro)
{
    const std::filesystem::path funcspath = createDatapack(pro._WorldName, pro._FileNameWithoutExtention);
    if (funcspath == std::filesystem::path())
    {
        return;
    }
    for (auto &func : pro._Functions)
    {
        std::string out;
        for (auto &instruction : func._Instructions)
        {
            out += instruction.name + " " + instruction.content + "\n";
        }
        std::string fname = func.name + ".mcfunction";
        std::filesystem::path fpath = funcspath / fname;
        std::ofstream ofs(fpath);
        ofs << out;
        ofs.close();
    }
}

void _debug_printVoidNodeBst(voidnode &vn)
{
    if (vn._Left == nullptr)
        return;
    if (vn._Left->index() == 0) // then it is a typed-void-pointer
    {
        shared_tvoidp tvoid = std::get<0>(*vn._Left);
        std::cout << '{' << tvoid.t << ',' << tvoid.v << "} ";
    }
    else
    {
        std::cout << '\n';
        _debug_printVoidNodeBst(std::get<1>(*vn._Left));
    }
    if (vn._Operator == bst_operator::NONE)
        return; // if false, there has to be a right node

    switch (vn._Operator)
    {
    case bst_operator::ADD:
        std::cout << '+';
        break;
    case bst_operator::SUB:
        std::cout << '-';
        break;
    case bst_operator::MUL:
        std::cout << '*';
        break;
    case bst_operator::DIV:
        std::cout << '/';
        break;
    case bst_operator::MOD:
        std::cout << '%';
        break;
    }
    std::cout << ' ';

    if (vn._Right->index() == 0) // then it is a typed-void-pointer
    {
        shared_tvoidp tvoid = std::get<0>(*vn._Right);
        std::cout << '{' << tvoid.t << ',' << tvoid.v << "}\n";
    }
    else
    {
        std::cout << '\n';
        _debug_printVoidNodeBst(std::get<1>(*vn._Right));
    }
}
std::vector<mcf::rs_variable> mcf::parseFunctionParameters(ltoken *tokens, int& _At, std::vector<std::string> &_Types, lex_error *out)
{
    std::vector<mcf::rs_variable> parameters;

    while (1)
    {
        ltoken t = tokens[_At];
        std::string name = t._Repr;

        if (tokens[++_At]._Type != COLON)
        {
            COMPILE_ERROR_P(out, EXPECTED_TYPE);
            return parameters;
        }
        std::string type = tokens[++_At]._Repr;

        int arrayCount = 0;
        while (tokens[++_At]._Type == OPEN_SQBR)
        {
            arrayCount++;
            if (tokens[++_At]._Type != CLOSED_SQBR)
            {
                COMPILE_ERROR_P(out, UNEXPECTED_TOKEN);
                return parameters;
            }
        }

        auto it = std::find(_Types.begin(), _Types.end(), type);
        if (std::find(_Types.begin(), _Types.end(), type) == _Types.end())
        {
            COMPILE_ERROR_P(out, UNKNOWN_TYPE);
            return parameters;
        }
        int _TypeID = it - _Types.begin();

        // templates are not supported as of these new versions.
        parameters.push_back(mcf::rs_variable{name, mcf::typedef_info{_TypeID, (arrayCount > 0), arrayCount}});

        if (tokens[_At]._Type != COMMA)
            return parameters;
    };
}
lex_error mcf::compileRsc(ltoken *tokens, int tokenCount, mcf::rscprogram *out)
{
    lex_error __STACK_TRACE = {0, 0, 0, 0, 0};

    int _At = -1;

    // the index of each of the elements here corrosponds to their type ID.
    std::vector<std::string> _Types = {"unknown", "int", "string", "float", "object"};
    std::unordered_map<std::string, function_byte_code> _Functions;

    // variables, and their values as voidnode bsts, so that we can parse them later.
    std::unordered_map<std::string, std::pair<mcf::rs_variable, std::shared_ptr<voidnode>>> _GlobalVariables;

    std::shared_ptr<function_byte_code> currentFunction = nullptr;
    int _ScopeCounter = 0;
    while ((++_At) < tokenCount)
    {
        ltoken *t = &tokens[_At];

        switch (t->_Type)
        {
        case OPEN_CBR:
            _ScopeCounter++;
            break;
        case CLOSED_CBR:
        {

            _ScopeCounter--;
            if(_ScopeCounter == 0)
            {
                _Functions.insert({currentFunction->_Name, *currentFunction});
                currentFunction.reset();
            }
            break;
        }
        case SEMI_COLON:
            continue;
        case TYPE_DEF:
        {
            if (tokens[++_At]._Type == WORD)
            {
                // it is a function.
                std::string name = tokens[_At]._Repr;

                if (tokens[++_At]._Type != OPEN_BR)
                    COMPILE_ERROR(UNEXPECTED_TOKEN);
                std::vector<rs_variable> parameters = parseFunctionParameters(tokens, ++_At, _Types, &__STACK_TRACE);

                if (__STACK_TRACE.ec != 0)
                    COMPILE_ERROR(__STACK_TRACE.ec);
                currentFunction = std::make_shared<function_byte_code>();
                currentFunction->_Name = name;
                auto &vars = currentFunction->_Variables;
                for (rs_variable parameter : parameters)
                {
                    if (std::count_if(vars.begin(), vars.end(), [&parameter](mcf::rs_variable v){
                        if(parameter.name == v.name) return 1; else return 0;
                    }) != 0)
                        COMPILE_ERROR(NAME_ALREADY_EXISTS);

                    currentFunction->_Instructions.push_back(mcf::byte_code_instruction{mcf::rs_instructions::CREATE,
                                                                                        {shared_tvoidp(parameter.name, STRING_TYPE_ID),
                                                                                         shared_tvoidp(parameter.type._TypeID, INT_TYPE_ID),
                                                                                         shared_tvoidp(parameter.type.arrayDimension, INT_TYPE_ID)}});
                }
                if(tokens[++_At]._Type != OPEN_CBR) COMPILE_ERROR(EXPECTED_FUNCTION_BODY);

            }
            break;
        }
        case WORD:
        {
            // the variable is mistaken for a parameter here.
            switch (tokens[++_At]._Type)
            {
            case COLON:
            {
                t->_Type = VARIABLE_DEF;

                std::string type = tokens[++_At]._Repr;
                auto it = std::find(_Types.begin(), _Types.end(), type);

                if (std::find(_Types.begin(), _Types.end(), type) == _Types.end())
                    COMPILE_ERROR(UNKNOWN_TYPE);

                int _TypeID = it - _Types.begin();

                int arrayCount = 0;
                while (_At + 1 < tokenCount && tokens[++_At]._Type == OPEN_SQBR)
                {
                    arrayCount++;
                    if (tokens[++_At]._Type != CLOSED_SQBR)
                        COMPILE_ERROR(UNEXPECTED_TOKEN);
                }

                if (tokens[_At]._Type != ASSIGN)
                    COMPILE_ERROR(UNEXPECTED_TOKEN);
                if (tokens[_At]._Type == SEMI_COLON)
                    COMPILE_ERROR(EXPECTED_VALUE);
                int begin = _At + 1;

                while (_At + 1 < tokenCount && tokens[_At++]._Type != SEMI_COLON)
                    ;

                if (tokens[--_At]._Type != SEMI_COLON)
                {
                    _At = begin; // for debugging purposes, we wouldnt want the error to be at the end of the file.
                    COMPILE_ERROR(EXPECTED_SEMICOLON);
                }
                int out = _At;

                voidnode expression = mexpreval(tokens, tokenCount, begin, _At, &__STACK_TRACE, &out);
                if (__STACK_TRACE.ec != 0)
                {
                    COMPILE_ERROR(__STACK_TRACE.ec);
                }
                _At = out;
                // std::cout << "vn-root:\n";
                // _debug_printVoidNodeBst(expression);
                std::string name = t->_Repr;

                if (currentFunction == nullptr)
                {
                    // global variable
                    if (_GlobalVariables.find(name) != _GlobalVariables.end())
                        COMPILE_ERROR(NAME_ALREADY_EXISTS);
                    _GlobalVariables.insert({name, std::pair<mcf::rs_variable, std::shared_ptr<voidnode>>{mcf::rs_variable{name,
                                                                    {_TypeID, (arrayCount > 0), arrayCount}},
                                                   std::make_shared<voidnode>(expression)}});
                }
                else
                {
                    // local variable
                    auto &vars = currentFunction->_Variables;
                    if (std::count_if(vars.begin(), vars.end(), [&name](mcf::rs_variable v){
                        if(name == v.name) return 1; else return 0;
                    }) != 0)
                        COMPILE_ERROR(NAME_ALREADY_EXISTS);
                    currentFunction->_Instructions.push_back({mcf::rs_instructions::CREATE,
                                                              {shared_tvoidp(name, STRING_TYPE_ID),
                                                               shared_tvoidp((arrayCount > 0), INT_TYPE_ID),
                                                               shared_tvoidp(arrayCount, INT_TYPE_ID)}});

                    currentFunction->_Instructions.push_back({mcf::rs_instructions::ASSIGN,
                                                              {shared_tvoidp(name, STRING_TYPE_ID),
                                                               shared_tvoidp(expression, UNEVALUATED_NODE_TREE_ID)}});
                    currentFunction->_Variables.push_back(mcf::rs_variable{name,
                                                                    {_TypeID, (arrayCount > 0), arrayCount}});
                }

                break;
            }
            case OPEN_BR:
            {
                // could be function or function call
                if(currentFunction == nullptr)
                {
                    // global function call, called before entry point, do we allow?
                    COMPILE_ERROR(UNSUPPORTED_FEATURE);
                }

                std::string name = t->_Repr;

                if (_Functions.find(name) == _Functions.end())
                    COMPILE_ERROR(UNDEFINED_TOKEN_IDENTIFIER);
                std::vector<voidnode> vns = m_evalparams(tokens, tokenCount, ++_At, &__STACK_TRACE);
                if (__STACK_TRACE.ec != 0)
                    COMPILE_ERROR(__STACK_TRACE.ec);

                for (voidnode& vn : vns)
                {
                    currentFunction->_Instructions.push_back({rs_instructions::PARAM, 
                    {shared_tvoidp(0, INT_TYPE_ID), 
                     shared_tvoidp(vn, UNEVALUATED_NODE_TREE_ID)
                    }});
                }
                currentFunction->_Instructions.push_back({rs_instructions::CALL,
                {
                    shared_tvoidp(name, STRING_TYPE_ID),
                    shared_tvoidp((int)vns.size(), INT_TYPE_ID)
                }});
                break;
            }
            case SEMI_COLON:
                continue;
            }
            break;
        }
        }
    }
    *out = rscprogram(); // TODO

    auto& sdei = out->_StdDeclEntryIndicies;

    if(_Functions.find("run") != _Functions.end()) sdei.push_back(0);

    if(sdei.size() == 0)
        COMPILE_ERROR(NO_ENTRY_POINT);

    // first one specified.
    out->_ChosenEntryIndex = sdei[0];

    for(auto& function : _Functions)
    {
        
    }

    return __STACK_TRACE;
}
lex_error mcf::buildRsc(rscprogram &p)
{
    writemcp(p);

    return lex_error{};
}
int mcf::findTrailingChar(char c, ltoken *tokens, int begin, int len)
{
    std::string repr;
    while (begin + 1 < len && (repr = tokens[++begin]._Repr)[0] != c || repr.length() > 1)
        ;

    return begin == len - 1 ? -1 : begin;
}
std::vector<voidnode> mcf::m_evalparams(ltoken* tokens, int len, int& _At, lex_error* out)
{
    // when this is called, the index of begin should be the first token in the first parameter.
    std::vector<voidnode> vns;
    do
    {
        int end = _At;
        while(end + 1 < len && tokens[++end]._Type != COMMA);

        if(end == len - 1) 
            return vns;

        voidnode vn = mcf::mexpreval(tokens, len, _At, end, out, &_At);
        vns.push_back(vn);

    } while (tokens[_At++]._Type == COMMA);
    
    if(tokens[--_At]._Type != CLOSED_BR)
    {
        COMPILE_ERROR_P(out, UNEXPECTED_TOKEN);
    }

    return vns;

}
voidnode mcf::mexpreval(ltoken *tokens, int len, int begin, int end, lex_error *out, int* counterOut)
{
    // when this is called, the index of begin should be the first token in the expression, or a '('.
    int _At = begin;
    counterOut = &_At;
    bool _Side = false; // left 0 right 1
    voidnode vncurrent;


    // TODO add function support
evaluate:
    if(_At >= end) return vncurrent;
    
    ltoken &current = tokens[_At];
    if (current._Type == OPEN_BR)
    {
        int c = findTrailingChar(')', tokens, _At, len);
        if (tokens[_At + 1]._Type == CLOSED_BR)
        {
            COMPILE_ERROR_P(out, EXPECTED_VALUE);
            return vncurrent;
        }
        if (!_Side)
        {
            // get all x amount of nodes. The first operation of left and right is assigned to the left/right side
            // of the node we are operating on, then we append all other nodes found to the tree,
            // mimicing the execution order of these nodes.
            voidnode left = mexpreval(tokens, len, begin + 1, c, out, counterOut);
            vncurrent._Left = std::make_shared<std::variant<tvoidp, voidnode>>(left);
        }
        else
        {
            voidnode right = mexpreval(tokens, len, begin + 1, c, out, counterOut);
            vncurrent._Right = std::make_shared<std::variant<tvoidp, voidnode>>(right);
        }
    }
    else
    {
        if (current._Type != STRING_LITERAL && current._Type != INTEGER_LITERAL && current._Type != FLOAT_LITERAL)
        {
            COMPILE_ERROR_P(out, UNEXPECTED_TOKEN);
            return vncurrent;
        }
        vncurrent._Left = std::make_shared<std::variant<tvoidp, bst<tvoidp>>>(tvoidp{current._Repr, static_cast<int>(current._Type)});
    }

    ltoken &op = tokens[++_At];
    char opc = op._Repr[0];

    if (strlen(op._Repr) != 1 || !IS_OP(opc))
    {
        // COMPILE_ERROR_P(out, EXPECTED_OPERATOR);
        return vncurrent;
    }

    switch (opc)
    {
    case '+':
        vncurrent._Operator = bst_operator::ADD;
        break;
    case '-':
        vncurrent._Operator = bst_operator::SUB;
        break;
    case '*':
        vncurrent._Operator = bst_operator::MUL;
        break;
    case '/':
        vncurrent._Operator = bst_operator::DIV;
        break;
    case '%':
        vncurrent._Operator = bst_operator::MOD;
        break;
    case ';':
        return vncurrent;
    default:
        COMPILE_ERROR_P(out, UNEXPECTED_TOKEN);
        return vncurrent;
    }

    _At++;
    if (!_Side)
    {
        // do right side
        _Side = !_Side;
        goto evaluate;
    }
    else
    {
        // we have done left and right, and there might another node beside the root node.
        opc = tokens[_At]._Repr[0];
        // root operators are operators that operate on two root nodes, ex : x = (44 + 2) * (12). The root
        // operator here is *. They are used so that
        switch (opc)
        {
        case '+':
            vncurrent._RootOperator = bst_operator::ADD;
            break;
        case '-':
            vncurrent._RootOperator = bst_operator::SUB;
            break;
        case '*':
            vncurrent._RootOperator = bst_operator::MUL;
            break;
        case '/':
            vncurrent._RootOperator = bst_operator::DIV;
            break;
        case '%':
            vncurrent._RootOperator = bst_operator::MOD;
            break;
        case ';':
            return vncurrent;
        default:
            COMPILE_ERROR_P(out, UNEXPECTED_TOKEN);
            return vncurrent;
        }

        _At++;
        goto evaluate;
    }
}
