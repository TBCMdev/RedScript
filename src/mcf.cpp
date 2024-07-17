#include "mcf.hpp"

std::filesystem::path createDatapack(const std::string& world, const std::string& name)
{
    std::filesystem::path path = DATAPACK_FOLDER(world) + name + "/pack.mcmeta";
    std::filesystem::create_directory(path.parent_path());

    std::ofstream ofs(path);
    ofs << MCMETA(version);
    ofs.close();

    path /= "functions";
    std::filesystem::create_directory(path);

    return path;
}
void mcf::fAddCommand(mcfunction& f, command& c)
{
    f._Instructions.push_back(c);
}
void mcf::pAddFunction(rscprogram& pro, mcfunction& f)
{
    pro._Functions.push_back(f);
}
void mcf::writemcp(rscprogram& pro)
{
    const std::filesystem::path dpath = createDatapack(pro._WorldName, pro._FileNameWithoutExtention);
    for(auto& func : pro._Functions)
    {
        std::string out;
        for(auto& instruction : func._Instructions)
        {
            out += instruction.name + " " + vecunpack(instruction.parameters) + "\n";
        }
        std::string fname = func.name + ".mcfunction";
        std::filesystem::path fpath = dpath / fname;
        std::ofstream ofs(fpath);
        ofs << out;
        ofs.close();
    }
}

lex_error mcf::compileRsc(ltoken* tokens,int tokenCount, mcf::rscprogram* out)
{
    lex_error __STACK_TRACE = {0, 0, 0, 0, 0};

    // todo
    int _At = -1;

    std::vector<std::string> _Types = {"int", "string", "float", "object", "unknown"};

    std::unordered_map<std::string, function_byte_code> _Functions;

    std::shared_ptr<function_byte_code> currentFunction = nullptr;

    while((++_At) < tokenCount)
    {
        ltoken* t = &tokens[_At];

        switch(t->_Type)
        {
            case NEWLINE:
                __STACK_TRACE.lineindex = _At;
                __STACK_TRACE.line++;
                __STACK_TRACE.cpos = 0;
                continue;
            case SEMI_COLON:
                continue;
            case TYPE_DEF:
            {
                if(tokens[++_At]._Type == WORD)
                {
                    // it is a function.
                    // TODO
                }
                break;
            }
            case WORD:
            {
                switch(tokens[++_At]._Type)
                {
                    case COLON:
                    {
                        t->_Type = VARIABLE_DEF;
                        if(std::count(_Types.begin(), _Types.end(), tokens[++_At]._Repr) == 0) COMPILE_ERROR(UNKNOWN_TYPE);

                        if(tokens[_At + 1]._Type == SEMI_COLON) COMPILE_ERROR(EXPECTED_VALUE);

                        while(tokens[++_At]._Type != SEMI_COLON);

                        // TODO
                        expreval();

                        if(currentFunction == nullptr)
                        {
                            // global variable
                        }

                        break;
                    }
                    case OPEN_BR:
                    {
                        // could be function or function call

                        break;
                    }
                    case SEMI_COLON:
                        continue;
                }
                break;
            }
        }
    }   
}
lex_error mcf::buildRsc(rscprogram& p)
{
    writemcp(p);

    return lex_error{};
}
int findTrailingChar(char c, ltoken* tokens, int begin, int len)
{
    std::string repr;
    while((repr = tokens[++begin]._Repr)[0] != c || repr.length() > 1);

    return begin == len ? -1 : begin; 
}
voidnode mexpreval(ltoken* tokens, int len, int begin, int end, lex_error* out)
{
    // when this is called, the index of begin should be the first token in the expression, or a '('.
    voidnode tree;

    int _At = begin;

    if (tokens[_At]._Type == OPEN_BR)
    {
        int c = findTrailingChar(')', tokens, begin, len);
        tree._Left = voidnode::makeNode(mexpreval(tokens, len, begin + 1, c, out));
    }
    // TODO

}
