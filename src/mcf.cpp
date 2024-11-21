#include "mcf.hpp"
#include "mcfms.hpp"

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
        std::cout << "Creating world directory: " << p << '\n';
        std::filesystem::create_directory(p); // create world folder
        printf("Creating general world files...");

        std::filesystem::create_directories(p / "region");
        std::filesystem::create_directories(p / "data");
        std::filesystem::create_directories(p / "playerdata");
        // Create a minimal level.dat file
        create_level_dat((p / "level.dat").string());
        return true;
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << '\n';
        return false;
    }
}

mcf::mcfunction mcf::fbc_to_mcfunc(function_byte_code &fbc, std::unordered_map<std::string, function_byte_code> &functions, lex_error *out)
{
    mcf::mcfunction function;
    function.name = fbc._Name;

    int _S = fbc._Instructions.size();

    int _IfStatementCounter = 0;
    int _IfStatementBegin = 0;
    std::string latestIfStatementClause;

    for (int i = 0; i < _S; i++)
    {
        mcf::byte_code_instruction &instruction = fbc._Instructions[i];
        switch (instruction._id)
        {
        case mcf::rs_instructions::CREATE:
        {
            if (instruction._Parameters.size() != 3)
                warn("Invalid byte code instruction parameter count received. If not resolved, your program will not behave normally.");
            auto &p0 = instruction._Parameters[0], p1 = instruction._Parameters[1], p2 = instruction._Parameters[2];
            if (p0.t != STRING_TYPE_ID ||
                p1.t != INT_TYPE_ID ||
                p2.t != INT_TYPE_ID)
            {
                warn("Invalid byte code instruction parameter types received. This is an internal error that has not been checked as a result of invalid code fed to the compiler.");
                return function;
            }

            int p2val = *(int *)p2.v.get();
            mcf::rs_variable tempv = {*(std::string *)p0.v.get(),
                                      typedef_info{
                                          (*(int *)p1.v.get()),
                                          (p2val > 0),
                                          p2val}};

            auto instructions = createLocalVariable(tempv);
            function._Instructions.insert(function._Instructions.end(), instructions.begin(), instructions.end());
            break;
        }
        case mcf::rs_instructions::ASSIGN:
        {
            if (instruction._Parameters.size() != 2)
                warn("Invalid byte code instruction parameter count received. If not resolved, your program will not behave normally.");
            auto &p0 = instruction._Parameters[0], p1 = instruction._Parameters[1];

            if (p0.t != STRING_TYPE_ID)
            {
                warn("Invalid byte code instruction parameter types received. This is an internal error that has not been checked as a result of invalid code fed to the compiler.");
                return function;
            }

            std::string &name = *(std::string *)p0.v.get();

            switch (p1.t)
            {

            case UNEVALUATED_NODE_TREE_ID:
            {
                voidnode &val = *(voidnode *)p1.v.get();

                auto instructions = assignVariableValue(name, val);
                function._Instructions.insert(function._Instructions.end(), instructions.begin(), instructions.end());
                break;
            }
            default:
            {
                auto instructions = assignVariableValue(name, p1);
                function._Instructions.insert(function._Instructions.end(), instructions.begin(), instructions.end());
                break;
            }
            }
            break;
        }
        case mcf::rs_instructions::CALL:
        {
            if (instruction._Parameters.size() < 1)
                warn("Invalid byte code instruction parameter count received. If not resolved, your program will not behave normally.");
            std::string name = *(std::string *)instruction._Parameters[0].v.get();

            std::unordered_map<std::string, mcf::function_byte_code>::iterator _function;

            if ((_function = functions.find(name)) == functions.end())
            {
                out->ec = UNEXPECTED_TOKEN;
                return function;
            }

            // function._SegmentBreaks.push_back(i);
            auto _begin = instruction._Parameters.begin() + 1;
            auto _end = instruction._Parameters.end();

            std::vector<shared_tvoidp> justParams;
            std::copy(_begin, _end, std::back_inserter(justParams));

            CMD_LIST prep;
            if (_function->second._Inbuilt)
            {
                std::string paramsOut;
                prep = prepareFunctionCall(_function->second, justParams, &paramsOut);

                function._Instructions.insert(function._Instructions.end(), prep.begin(), prep.end());

                // call the respected function with the static parameters
                // todo , how to do dynamic variable parameters? TODO: Inbuilt function handling, write them!

                if (_IfStatementCounter > 0)
                    function._Instructions.push_back({"run", _function->first + ' ' + paramsOut});
                else
                    function._Instructions.push_back({_function->first, paramsOut});

                // function._Instructions.push_back({"function", "rs_garbage"});

                break;
            }
            else
            {
                prep = prepareFunctionCall(_function->second, justParams, nullptr);

                function._Instructions.insert(function._Instructions.end(), prep.begin(), prep.end());

                // call the respected function with the parameter count
                function._Instructions.push_back({"function", _function->first + ' ' + std::to_string(instruction._Parameters.size() - 1)});
                function._Instructions.push_back({"function", "rs_garbage"});
            }
            break;
        }
        case mcf::rs_instructions::RET:
        {
            if (instruction._Parameters.size() < 1)
            {
                function._Instructions.push_back({"return", "0"});
                break;
            }
            voidnode &p1 = *(voidnode *)instruction._Parameters[0].v.get();
            auto instructions = evalVoidnode(p1, &mcf::registers::ret);
            function._Instructions.insert(function._Instructions.end(), instructions.first.begin(), instructions.first.end());

            // return 1, indicating that we have returned a value and stored it in the ret register.
            // TODO: copyRegister 1 and 2, evalVoidNode with functions (impl return cmd), and function overloading in future
            function._Instructions.push_back({"return", "1"});

            break;
        }
        case mcf::rs_instructions::CMP:
        {
            if (instruction._Parameters.size() != 3)
            {
                warn("Invalid byte code instruction parameter count received. If not resolved, your program will not behave normally.");
                break;
            }
            _IfStatementCounter++;

            voidnode &_lhs = *(voidnode *)instruction._Parameters[0].v.get(), _rhs = *(voidnode *)instruction._Parameters[2].v.get();
            std::string _cop = *(std::string *)instruction._Parameters[1].v.get();

            int ltype = getVoidnodeType(_lhs);
            int rtype = getVoidnodeType(_rhs);

            if (ltype != rtype)
            {
                out->ec = COMPARISON_TYPE_MISMATCH;
                return function;
            }

            auto lhsresult = evalVoidnode(_lhs);
            auto rhsresult = evalVoidnode(_rhs);
            function._Instructions.insert(function._Instructions.end(), lhsresult.first.begin(), lhsresult.first.end());
            function._Instructions.insert(function._Instructions.end(), rhsresult.first.begin(), rhsresult.first.end());

            std::string operation = _cop == "==" ? "if" : _cop == "!=" ? "unless"
                                                                       : "";

            if (operation == "")
            {
                // we are not doing a direct comparison
                if (ltype != INT_TYPE_ID || true)
                {
                    out->ec = NOT_SUPPORTED;
                    return function;
                }
                // TODO
            }
            else
            {
                switch (ltype)
                {
                case STRING_TYPE_ID:
                case FLOAT_TYPE_ID:
                case INT_TYPE_ID:
                {
                    // todo
                    // function._Instructions.push_back({"data", MC_DATA_SET_FROM("storage", RS_PROGRAM_ROOT " _cmpr[" + std::to_string(_IfStatementCounter - 1) + "]", VARIN(lhsresult.second->path()))});
                    function._Instructions.push_back({"execute", (("store result storage " RS_PROGRAM_ROOT " _cmpr[" + std::to_string(_IfStatementCounter - 1) + "] run data ") + MC_DATA_SET_FROM("storage", VARIN(lhsresult.second->path()), VARIN(rhsresult.second->path())))});
                    function._Instructions.push_back({"execute", "store result score _PROGDATA _CMPRESULT run data get " RS_PROGRAM_ROOT " _cmpr[" + std::to_string(_IfStatementCounter - 1) + "]"});
                    latestIfStatementClause = operation + " score _PROGDATA _CMPRESULT matches 1 ";
                    break;
                }
                default:
                {
                    warn("Direct Comparisons are not supported for types other than string, int, or float/double.");
                    break;
                }
                }
            }

            _IfStatementBegin = function._Instructions.size();
            break;
        }
        case mcf::rs_instructions::CMP_END:
        {
            _IfStatementCounter--;
            break;
        }
        default:
        {
            warn("unknown/unsupported byte code instruction received.");
            break;
        }
        }
        if (_IfStatementCounter <= 0)
            continue;

        // we need to modify instructions so that they conform with the if statements query. (execute if/unless...)
        for (int j = _IfStatementBegin; j < function._Instructions.size(); j++)
        {
            mcf::command &instruction = function._Instructions[j];
            instruction.content = latestIfStatementClause + instruction.name + ' ' + instruction.content;
            instruction.name = "execute";
        }
    }
    return function;
}

std::filesystem::path mcf::createDatapack(const std::string &world, const std::string &name)
{
    std::string dpf = DATAPACK_FOLDER(world);
    std::filesystem::path worldPath = std::filesystem::path(dpf).parent_path().parent_path();
    std::cout << "Found world path: " << worldPath << '\n';
    if (!std::filesystem::exists(worldPath)) // if the world doesnt exist
    {
        std::cout << "[INFO] World " << world << " does not exist. Creating...\n";
        if (!mcf::createVoidWorld(worldPath))
        {
            std::cout << "[ERROR] Could not create world. Aborting...\n";
            exit(-1);
        }
    }
    std::filesystem::create_directory(dpf);
    std::filesystem::path path = dpf + name + "/pack.mcmeta";
    std::filesystem::create_directory(path.parent_path());

    std::ofstream ofs(path);
    ofs << MCMETA(version);
    ofs.close();

    std::filesystem::path funcspath = path.parent_path() / "functions";
    std::filesystem::create_directory(funcspath);

    return funcspath;
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
        std::cout << "[ERROR] Error occured while creating datapack.\n";
        return;
    }
    for (auto &func : pro._Functions)
    {
        std::string out;
        int i = 0;
    segWrite:
        bool seg = false;
        int segc = 0;
        for (; i < func._Instructions.size(); i++)
        {
            auto &instruction = func._Instructions[i];
            if (instruction.content.ends_with(' '))
                instruction.content.pop_back();
            out += instruction.name + " " + instruction.content + "\n";

            if (std::count(func._SegmentBreaks.begin(), func._SegmentBreaks.end(), i) == 1)
            {
                // we have a segment at this line
                seg = true;
                break;
            }
        }
        std::string fname;
        if (!seg)
            fname = func.name + ".mcfunction";
        else
        {
            fname = func.name + "-" + std::to_string(segc++) + ".mcfunction";
        }
        std::filesystem::path fpath = funcspath / fname;
        std::ofstream ofs(fpath);
        ofs << out;
        ofs.close();

        if (seg)
        {
            seg = false;
            goto segWrite;
        }
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
std::vector<mcf::rs_variable> mcf::parseFunctionParameters(ltoken *tokens, int &_At, std::vector<std::string> &_Types, lex_error *out)
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

        _At++;
    };
}

lex_error mcf::compileRsc(ltoken *tokens, int tokenCount, std::shared_ptr<mcf::rscprogram> &out)
{
    lex_error __STACK_TRACE = {0, 0, 0, 0, 0};
    // todo, function calls
    int _At = -1;
    int _IfStatementNestCount = 0;

    // the index of each of the elements here corrosponds to their type ID.
    std::vector<std::string> _Types = {"void", "int", "string", "float", "object", "selector", "nselector"};
    std::unordered_map<std::string, function_byte_code> _Functions;
    // variables, and their values as voidnode bsts, so that we can parse them later.
    std::unordered_map<std::string, std::pair<mcf::rs_variable, std::shared_ptr<voidnode>>> _GlobalVariables;
    std::shared_ptr<mcf::rs_namespace> currentNamespace = nullptr;
    std::shared_ptr<function_byte_code> currentFunction = nullptr;

    mcf::rsp_context _Context{_Functions, currentFunction};

    int _ScopeCounter = 0;
    while ((++_At) < tokenCount)
    {
        ltoken *t = &tokens[_At];

        switch (t->_Type)
        {
        case OPEN_CBR:
            _ScopeCounter++;
            continue;
        case CLOSED_CBR:
        {
            _ScopeCounter--;
            if (_IfStatementNestCount > 0)
                _IfStatementNestCount--;
            else
            {
                if (currentFunction != nullptr)
                    currentFunction->_Instructions.push_back({rs_instructions::CMP_END, {}});
            }
            if (_ScopeCounter == 0)
            {
                _Functions.insert({currentFunction->_Name, *currentFunction});
                currentFunction.reset();
            }
            continue;
        }
        case HASHTAG:
        {
            // we do not parse macros here. Continue.
            _At += 2;
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
                _At++;
                std::vector<rs_variable> parameters = parseFunctionParameters(tokens, _At, _Types, &__STACK_TRACE);

                if (__STACK_TRACE.ec < 0)
                    COMPILE_ERROR_RAW;
                currentFunction = std::make_shared<function_byte_code>();
                currentFunction->_Name = name;
                currentFunction->_Parameters = parameters;
                auto &vars = currentFunction->_Variables;
                for (rs_variable parameter : parameters)
                {
                    if (std::count_if(vars.begin(), vars.end(), [&parameter](mcf::rs_variable v)
                                      {
                        if(parameter.name == v.name) return 1; else return 0; }) != 0)
                        COMPILE_ERROR(NAME_ALREADY_EXISTS);

                    currentFunction->_Instructions.push_back(mcf::byte_code_instruction{mcf::rs_instructions::CREATE,
                                                                                        {shared_tvoidp(parameter.name, STRING_TYPE_ID),
                                                                                         shared_tvoidp(parameter.type._TypeID, INT_TYPE_ID),
                                                                                         shared_tvoidp(parameter.type.arrayDimension, INT_TYPE_ID)}});
                }
                if (tokens[++_At]._Type != OPEN_CBR)
                {
                    if (tokens[_At]._Type == SEMI_COLON || (seq(tokens[_At]._Repr, "__inbuilt") && tokens[_At + 1]._Type == SEMI_COLON))
                    {
                        // we are null defining the function, it is inbuilt
                        currentFunction->_Instructions.clear();
                        currentFunction->_Inbuilt = true;
                        _ScopeCounter--;
                        std::unordered_map<std::string, mcf::function_byte_code>::iterator f;
                        if ((f = _Functions.find(currentFunction->_Name)) != _Functions.end())
                        {
                            // we are overloading the function?
                            if (f->second._Parameters == currentFunction->_Parameters)
                                COMPILE_ERROR(FUNCTION_REDECLARATION);

                            COMPILE_ERROR(NOT_SUPPORTED);
                        }
                        _Functions.insert({currentFunction->_Name, *currentFunction});
                        currentFunction.reset();
                    }
                    else
                        COMPILE_ERROR(EXPECTED_FUNCTION_BODY);
                }

                _ScopeCounter++; // we are in function body.
            }
            break;
        }
        case WORD:
        {
            if (strcmp(t->_Repr, "return") == 0)
            {
                int c = _At + 1;

                if (!currentFunction)
                    COMPILE_ERROR(INVALID_TOKEN_LOCATION);

                if (tokens[c]._Type == SEMI_COLON)
                {
                    currentFunction->_Instructions.push_back(byte_code_instruction{mcf::rs_instructions::RET, {}});
                    break;
                }

                while (tokens[++c]._Type != SEMI_COLON)
                    ;

                // if a function turns out to be the output of mexpreval here,
                // the function call in the bst will already assign its return to the RET register.
                voidnode retVal = mexpreval(tokens, tokenCount, _At + 1, c, &__STACK_TRACE, &_At, _Context);

                currentFunction->_Instructions.push_back(byte_code_instruction{mcf::rs_instructions::RET, {shared_tvoidp(retVal, UNEVALUATED_NODE_TREE_ID)}});
                break;
            }
            else if (strcmp(t->_Repr, "if") == 0)
            {
                if (!currentFunction)
                    COMPILE_ERROR(INVALID_TOKEN_LOCATION);

                _IfStatementNestCount++;

                int start = _At, brc = 1;
                ltoken &tat = tokens[_At];
                if (tokens[++_At]._Type != OPEN_BR)
                    COMPILE_ERROR(UNEXPECTED_TOKEN);

                while ((tat = tokens[++_At])._Type != COMPARISON_OPERATOR && brc > 0)
                {
                    if (tat._Type == CLOSED_BR)
                        brc--;
                    else if (tat._Type == OPEN_BR)
                        brc++;
                }

                voidnode _left = mexpreval(tokens, tokenCount, start + 1, _At, &__STACK_TRACE, &_At, _Context);

                ltoken &comparison = tokens[_At];
                if (comparison._Type != COMPARISON_OPERATOR)
                    COMPILE_ERROR(UNEXPECTED_TOKEN);

                start = _At;
                brc = 1;
                while ((tat = tokens[++_At])._Type != COMPARISON_OPERATOR && brc > 0)
                {
                    if (tat._Type == CLOSED_BR)
                        brc--;
                    else if (tat._Type == OPEN_BR)
                        brc++;
                }

                voidnode _right = mexpreval(tokens, tokenCount, start + 1, _At, &__STACK_TRACE, &_At, _Context);

                if (tokens[_At]._Type != OPEN_CBR)
                    COMPILE_ERROR(EXPECTED_FUNCTION_BODY); // expected if body...
                _ScopeCounter++;

                currentFunction->_Instructions.push_back({rs_instructions::CMP, {shared_tvoidp(_left, UNEVALUATED_NODE_TREE_ID), shared_tvoidp(std::string(comparison._Repr), CMP_OPERATOR_ID), shared_tvoidp(_right, UNEVALUATED_NODE_TREE_ID)}});
                break;
            }

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

                // if it is a function call, goto parseFunctionCall, but also find
                // a way to pipe the result of the ret register to the variables value.
                voidnode expression = mexpreval(tokens, tokenCount, begin, _At, &__STACK_TRACE, &out, _Context);
                if (__STACK_TRACE.ec < 0)
                    COMPILE_ERROR_RAW;
                
                std::string name = t->_Repr;
                if (expression._Left)
                {
                    int exprType = UNEVALUATED_NODE_TREE_ID;

                    shared_tvoidp *loneVal;
                    if (!expression._Right && expression._Left->index() == 0)
                    {
                        loneVal = &std::get<0>(*expression._Left);
                        exprType = loneVal->t;
                    }
                    if (__STACK_TRACE.ec != 0)
                        COMPILE_ERROR_RAW;
                    _At = out - 1;

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
                        if (std::count_if(vars.begin(), vars.end(), [&name](mcf::rs_variable v)
                                          {if(name == v.name) return 1; else return 0; }) != 0)
                            COMPILE_ERROR(NAME_ALREADY_EXISTS);
                        currentFunction->_Instructions.push_back({mcf::rs_instructions::CREATE,
                                                                  {shared_tvoidp(name, STRING_TYPE_ID),
                                                                   shared_tvoidp(_TypeID, INT_TYPE_ID),
                                                                   shared_tvoidp(arrayCount, INT_TYPE_ID)}});
                        if (loneVal)
                        {
                            currentFunction->_Instructions.push_back({mcf::rs_instructions::ASSIGN,
                                                                      {shared_tvoidp(name, STRING_TYPE_ID),
                                                                       *loneVal}});
                        }
                        else
                        {
                            currentFunction->_Instructions.push_back({mcf::rs_instructions::ASSIGN,
                                                                      {shared_tvoidp(name, STRING_TYPE_ID),
                                                                       shared_tvoidp(expression, UNEVALUATED_NODE_TREE_ID)}});
                        }
                        currentFunction->_Variables.push_back(mcf::rs_variable{name,
                                                                               {_TypeID, (arrayCount > 0), arrayCount}});
                    }
                }
                else
                {
                    // if it is a function call on its own
                    // if we were to assign the value null to this variable, it would not be appended to the back of the instruction
                    // list, it would be inserted before the last element as the last element is the call instruction to the function
                    // which will initialize its value.
                    mcf::rs_variable var{name, {_TypeID, (arrayCount > 0), arrayCount}};
                    currentFunction->_Variables.push_back(var);
                }
                break;
            }
            case OPEN_BR:
            {
                // could be function or function call
                if (currentFunction == nullptr)
                {
                    // global function call, called before entry point, do we allow?
                    COMPILE_ERROR(UNSUPPORTED_FEATURE);
                }

                std::string name = t->_Repr;

                if (_Functions.find(name) == _Functions.end())
                    COMPILE_ERROR(UNDEFINED_TOKEN_IDENTIFIER);
                _At++;
                std::vector<voidnode> vns = m_evalparams(tokens, tokenCount, _At, &__STACK_TRACE, _Context);
                if (__STACK_TRACE.ec != 0)
                    COMPILE_ERROR_RAW;

                std::vector<shared_tvoidp> callParameters{shared_tvoidp(name, STRING_TYPE_ID)};

                for (voidnode &vn : vns)
                {
                    // we already know the type from the function sig
                    callParameters.push_back(shared_tvoidp(vn, UNEVALUATED_NODE_TREE_ID));
                }
                currentFunction->_Instructions.push_back({rs_instructions::CALL,
                                                          callParameters});
                break;
            }
            case SEMI_COLON:
                continue;
            }
            break;
        }
        }
    }

    out = std::make_shared<rscprogram>(); // TODO
    auto &sdei = out->_StdDeclEntryIndicies;

    _At--; // goto last token.
    if (_Functions.find("run") != _Functions.end())
        sdei.push_back(0);
    if (sdei.size() == 0)
        COMPILE_ERROR(NO_ENTRY_POINT);

    // first one specified.
    out->_ChosenEntryIndex = sdei[0];
    for (auto &function : _Functions)
    {
        if (function.second._Inbuilt)
            continue;

        out->_Functions.push_back(fbc_to_mcfunc(function.second, _Functions, &__STACK_TRACE));
        if (__STACK_TRACE.ec != 0)
            COMPILE_ERROR_RAW;
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
std::vector<voidnode> mcf::m_evalparams(ltoken *tokens, int len, int &_At, lex_error *out, mcf::rsp_context &_Context)
{
    // when this is called, the index of begin should be the first token in the first parameter.
    std::vector<voidnode> vns;
    do
    {
        if (tokens[_At]._Type == COMMA)
            _At ++;
        int end = _At;
        while (end + 1 < len && (tokens[end + 1]._Type != COMMA && tokens[end + 1]._Type != CLOSED_BR))
            end++;

        if (end == len - 1)
            return vns;

        voidnode vn = mcf::mexpreval(tokens, len, _At, end, out, &_At, _Context);

        if (out->ec < 0)
            return vns;

        vns.push_back(vn);

    } while (tokens[++_At]._Type == COMMA);

    // error here
    if (tokens[_At]._Type != CLOSED_BR)
        COMPILE_ERROR_P(out, UNEXPECTED_TOKEN);
    return vns;
}
voidnode mcf::mexpreval(ltoken *tokens, int len, int begin, int end,
                        lex_error *out, int *counterOut, mcf::rsp_context &_Context)
{
    // when this is called, the index of begin should be the first token in the expression, or a '('.
    int &_At = *counterOut;
    _At = begin;
    bool _Side = false; // left 0 right 1
    voidnode vncurrent;

evaluate:
    if (_At > end)
        return vncurrent;

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
            voidnode left = mexpreval(tokens, len, begin + 1, c, out, counterOut, _Context);
            vncurrent._Left = std::make_shared<std::variant<shared_tvoidp, voidnode>>(left);
        }
        else
        {
            voidnode right = mexpreval(tokens, len, begin + 1, c, out, counterOut, _Context);
            vncurrent._Right = std::make_shared<std::variant<shared_tvoidp, voidnode>>(right);
        }
    }
    else if (_At + 1 < end && tokens[_At + 1]._Type == OPEN_BR)
    {
        if (!_Context.currentFunction)
        {
            COMPILE_ERROR_P(out, UNSUPPORTED_FEATURE);
            return vncurrent;
        }
        if (vncurrent._Left.get())
        {
            // functions in this are only supported if they are alone in their expression.
            COMPILE_ERROR_P(out, UNSUPPORTED_FEATURE);
            return vncurrent;
        }

        std::string name = current._Repr;

        if (_Context._Functions.find(name) == _Context._Functions.end())
        {
            COMPILE_ERROR_P(out, UNDEFINED_TOKEN_IDENTIFIER);
            return vncurrent;
        }
        _At++;
        std::vector<voidnode> vns = m_evalparams(tokens, len, _At, out, _Context);
        if (out->ec < 0)
        {
            COMPILE_ERROR_RAW_P(out);
            return vncurrent;
        }
        std::vector<shared_tvoidp> callParameters{shared_tvoidp(name, STRING_TYPE_ID)};

        for (voidnode &vn : vns)
        {
            // we already know the type from the function sig
            callParameters.push_back(shared_tvoidp(vn, UNEVALUATED_NODE_TREE_ID));
        }
        _Context.currentFunction->_Instructions.push_back({rs_instructions::CALL,
                                                           callParameters});
        return vncurrent;
    }
    else
    {
        if (current._Type != STRING_LITERAL && current._Type != INTEGER_LITERAL && current._Type != FLOAT_LITERAL && current._Type != SELECTOR)
        {
            // problem: currentFunction is empty?
            if (_Context.currentFunction)
            {
                auto &vars = _Context.currentFunction->_Variables;
                auto &pars = _Context.currentFunction->_Parameters;
                if (std::count_if(vars.begin(), vars.end(), [&current](mcf::rs_variable &v)
                                  { return v.name == current._Repr; }) ||
                    std::count_if(pars.begin(), pars.end(), [&current](mcf::rs_variable &p)
                                  { return p.name == current._Repr; }) > 0)
                    goto skip;
            }
            /**
             * if(tokens[_At + 1]._Type == COLON)
                {
                    // nselector, not variable def
                    std::vector<std::string> selectors{t->_Repr};

                    while (tokens[_At]._Type == COLON && tokens[++_At]._Type == COLON)
                        selectors.push_back(tokens[_At++]._Repr);


                }
             *
             */
            COMPILE_ERROR_P(out, UNEXPECTED_TOKEN);
            return vncurrent;
        }
    skip:
        std::string repr = current._Repr;
        vncurrent._Left = std::make_shared<std::variant<shared_tvoidp, voidnode>>(shared_tvoidp{repr, static_cast<int>(current._Type)});
    }



    ltoken &op = tokens[_At + 1];
    char opc = op._Repr[0];

    if (strlen(op._Repr) != 1 || !IS_OP(opc))
        return vncurrent;

    _At++;
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
