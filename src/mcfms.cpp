#include "mcfms.hpp"

#pragma region maths
std::pair<CMD_LIST, mcf::mcf_register*> _ARRoperate(mcf::mcf_register& res, shared_tvoidp& r, std::string operation)
{
    CMD_LIST _Instructions;

    // need edge case for variables, TODO
    if (r.t == INT_TYPE_ID)
    {
        // both are ints, exec command too compilicated for macros
        auto* reg = mcf::registers::findFreeARR();

        if (!reg) // maybe create new register if this happens?
            return std::make_pair(_Instructions, &mcf::registers::null);
        
        // mcf::command cmd = {"execute", "store result score _RSRDC_ " + reg->display + " run data get storage " + RS_PROGRAM_ROOT + " "};
        _Instructions.push_back({"scoreboard", MC_SCOREBOARD_PLAYER_SET("_RSRDC_", VARIN(reg->display), VARIN_L(std::to_string(*(int*)r.v.get())))});
        auto* reg2 = mcf::registers::findFreeARR();

        if (!reg2) // maybe create new register if this happens?
            return std::make_pair(_Instructions, &mcf::registers::null);
        
        _Instructions.push_back({"scoreboard", MC_SCOREBOARD_OPERATION("_RSRDC_", VARIN(reg->display), VARIN(operation), "_RSRDC_", VARIN_L(reg2->display))});
        return std::make_pair(_Instructions, reg);
    }
    return std::make_pair(_Instructions, &mcf::registers::null);
}
std::pair<CMD_LIST, mcf::mcf_register *> _ARRoperate(shared_tvoidp &l, shared_tvoidp &r, std::string operation)
{
    CMD_LIST _Instructions;

    switch (l.t)
    {
    case INT_TYPE_ID:
    {
        if (r.t == INT_TYPE_ID)
        {
            auto regs = mcf::registers::findTwoOperableRegisters();
            if (regs.first == regs.second)
                return std::make_pair(_Instructions, &mcf::registers::null);

            CMD_LIST setup_r1 = setRegister(*regs.first, l);
            CMD_LIST setup_r2 = setRegister(*regs.second, r);

            _Instructions.insert(_Instructions.end(), setup_r1.begin(), setup_r1.end());
            _Instructions.insert(_Instructions.end(), setup_r2.begin(), setup_r2.end());

            // scoreboard operation!
            std::string scmd = MC_SCOREBOARD_OPERATION("_RSRDC_", VARIN(regs.first->display), VARIN(operation), "_RSRDC_", VARIN_L(regs.second->display));

            _Instructions.push_back(mcf::command{"scoreboard", scmd});
            // result will be stored in regs.first register.
            return std::make_pair(_Instructions, regs.first);
        }
        // TODO
        break;
    }
    }
    return std::make_pair(_Instructions, &mcf::registers::null);
}

#pragma endregion

std::pair<CMD_LIST, mcf::mcf_register *> evalVoidnode(voidnode &vn)
{
    CMD_LIST _Instructions;
    int lIndex, rIndex;
    if ((lIndex = vn._Left->index()) == 1)
    {
        auto l = std::get<1>(*vn._Left);
        auto ret = evalVoidnode(l);

        _Instructions.insert(_Instructions.end(), ret.first.begin(), ret.first.end());
        
        if ((rIndex = vn._Right->index()) != 1)
        {
            shared_tvoidp& r = std::get<0>(*vn._Right);

            auto l_add_r = _ARRoperate(*ret.second, r, bst_operator_str(vn._Operator));
            _Instructions.insert(_Instructions.end(), l_add_r.first.begin(), l_add_r.first.end());

            return std::make_pair(_Instructions, l_add_r.second);

        }
    }
    else
    {
        shared_tvoidp& l = std::get<0>(*vn._Left);
        
        if(!vn._Right.get())
        {
            auto* reg = mcf::registers::findFreeGPR();
            auto rset = setRegister(*reg, l);

            _Instructions.insert(_Instructions.end(), rset.begin(), rset.end());
            return std::make_pair(_Instructions, reg);
        }
        
        // todo: evaluate variables and function calls below this statement
        
        if(vn._Right->index() == 0)
        {   
            shared_tvoidp& r = std::get<0>(*vn._Right);
            if(r.t == l.t && r.t != VARIABLE_TYPE_ID)
            {
                // they are both raw nodes with just numbers, we can add them at compile time
                int toper = 0;
                switch(vn._Operator)
                {
                    case bst_operator::ADD: l.v.reset(&(toper = (*(int*)l.v.get()) + (*(int*)r.v.get()))); break;
                    case bst_operator::SUB: l.v.reset(&(toper = (*(int*)l.v.get()) - (*(int*)r.v.get()))); break;
                    case bst_operator::MUL: l.v.reset(&(toper = (*(int*)l.v.get()) * (*(int*)r.v.get()))); break;
                    case bst_operator::DIV: l.v.reset(&(toper = (*(int*)l.v.get()) / (*(int*)r.v.get()))); break;
                    case bst_operator::MOD: l.v.reset(&(toper = (*(int*)l.v.get()) % (*(int*)r.v.get()))); break;
                }
                // invalid number now, it has been operated on.
                r.v.reset();

                auto* reg = mcf::registers::findFreeGPR();
                auto rset = setRegister(*reg, l);
                _Instructions.insert(_Instructions.end(), rset.begin(), rset.end());

                return std::make_pair(_Instructions, reg);
            }
            // handle variables
            return std::make_pair(_Instructions, &mcf::registers::null);

        }
        else
        {
            auto r = std::get<1>(*vn._Right);
            auto ret = evalVoidnode(r);

            _Instructions.insert(_Instructions.end(), ret.first.begin(), ret.first.end());

            auto l_add_r = _ARRoperate(*ret.second, l, bst_operator_str(vn._Operator));
            _Instructions.insert(_Instructions.end(), l_add_r.first.begin(), l_add_r.first.end());

            return std::make_pair(_Instructions, l_add_r.second);
        }
    }
    return std::make_pair(_Instructions, &mcf::registers::null);
}

std::pair<CMD_LIST, mcf::mcf_register*> getRegister(mcf::mcf_register& reg)
{
    // todo
    return std::make_pair<CMD_LIST, mcf::mcf_register*>({}, nullptr);
}
CMD_LIST setRegister(mcf::mcf_register& reg, shared_tvoidp val)
{
    CMD_LIST _Instructions;
    switch(val.t)
    {
        case INT_TYPE_ID:
        {
            _Instructions.push_back({"scoreboard", MC_SCOREBOARD_PLAYER_SET("_RSRDC_", VARIN(reg.display), VARIN_L(std::to_string(*(int*)val.v.get())))});
            break;
        }
        case STRING_TYPE_ID:
        {
            _Instructions.push_back({"data", MC_DATA_SET_VALUE("storage", RS_PROGRAM_ROOT, "registers[\"" VARIN(std::string(reg.display)) "\"]", VARIN_L(*(std::string*)val.v.get()))});
            break;
        }
        default:
        {
            break;
        }
    }
    return _Instructions;
}
CMD_LIST createLocalVariable(mcf::rs_variable &variable)
{
    return {mcf::command{"data", MC_DATA_MERGE(RS_PROGRAM_ROOT, "variables", RS_VARIABLE_JSON(variable, 0))},
            mcf::command{"execute", "store result storage " RS_PROGRAM_ROOT " variables[\"" + variable.name + "\"].scope run scoreboard players get _PROGDATA _SCOPE"}};
}
CMD_LIST prepareFunctionCall(mcf::function_byte_code& function, std::vector<shared_tvoidp>& pvals, std::string* inbFuncParamsOut = nullptr)
{
    CMD_LIST cmdlist = {{"scoreboard", "players add _PROGDATA _SCOPE 1"}};
    
    for(int i = 0; i < pvals.size(); i++)
    {
        shared_tvoidp& val = pvals[i];
        mcf::rs_variable var = function._Parameters[i];


        switch(val.t)
        {
            case INT_TYPE_ID:
            {
                auto cval = *(int*)val.v.get();
                if(inbFuncParamsOut)
                {
                    *inbFuncParamsOut += std::to_string(cval) + ' ';
                    break;
                }
                mcf::command vset = {"data"};
                vset.content = MC_DATA_SET_VALUE("storage", RS_PROGRAM_ROOT, "variables[\"" + var.name + "\"].data", VARIN_L(std::to_string(cval)));
                cmdlist.push_back(vset);
                
                break;
            }
            case STRING_TYPE_ID:
            {
                auto cval = *(std::string*)val.v.get();
                if(inbFuncParamsOut)
                {
                    *inbFuncParamsOut += '"' + cval + "\" ";
                    break;
                }
                mcf::command vset = {"data"};
                vset.content = MC_DATA_SET_VALUE("storage", RS_PROGRAM_ROOT, "variables[\"" + var.name + "\"].data", VARIN_L(cval));
                cmdlist.push_back(vset);
                
                break;
            }
            case UNEVALUATED_NODE_TREE_ID:
            {
                // todo for inbFuncParamsOut!!! Dont know how to do
                auto cmds = assignVariableValue(var.name, *(voidnode*)val.v.get());
                cmdlist.insert(cmdlist.end(), cmds.begin(), cmds.end());
                break;
            }
            default:
            {
                warn("preparing function parameter failed, unknown type. Some parameters of this function call will be null.");
                break;
            }
        }
    }

    return cmdlist;
}
CMD_LIST createLocalVariable(mcf::rs_variable &variable, int _scope)
{
    return {mcf::command{"data", MC_DATA_MERGE(RS_PROGRAM_ROOT, "variables", RS_VARIABLE_JSON(variable, _scope))}};
}
CMD_LIST assignVariableValue(std::string name, voidnode &value)
{
    CMD_LIST cmdlist;
    // CMD_LIST cmdlist = {mcf::command{"data", MC_DATA_MERGE(RS_PROGRAM_ROOT, "variables", RS_VARIABLE_JSON(variable,_scope))}};

    auto ret = evalVoidnode(value);
    cmdlist.insert(cmdlist.end(), ret.first.begin(), ret.first.end());

    switch(ret.second->type)
    {
        case mcf::register_type::INTEGER_ONLY:
        {
            cmdlist.push_back(mcf::command{"execute", "store result storage " RS_PROGRAM_ROOT " variables[\"" + name + "\"].data int 1" + " run scoreboard players get _RSRDC_ " + ret.second->display});
            break;
        }
        default:
        {
            cmdlist.push_back(mcf::command{"data", MC_DATA_SET_FROM("storage", RS_PROGRAM_ROOT " variables[\"" + name + "\"].data", "storage " RS_PROGRAM_ROOT " registers[" VARIN(std::to_string(ret.second->id)) "]")});
            break;
        }
    }
    return cmdlist;
}
CMD_LIST assignVariableValue(std::string name, shared_tvoidp& constVal)
{
    CMD_LIST cmdlist;
    // CMD_LIST cmdlist = {mcf::command{"data", MC_DATA_MERGE(RS_PROGRAM_ROOT, "variables", RS_VARIABLE_JSON(variable,_scope))}};

    switch(constVal.t)
    {
        case INT_TYPE_ID:
        {
            cmdlist.push_back(mcf::command{"data", MC_DATA_SET_VALUE("storage", RS_PROGRAM_ROOT, "variables[\"" + name + "\"].data ", VARIN_L(std::to_string(*(int*)constVal.v.get())))});
            break;
        }
        case STRING_TYPE_ID:
        {
            cmdlist.push_back(mcf::command{"data", MC_DATA_SET_VALUE("storage", RS_PROGRAM_ROOT, "variables[\"" + name + "\"].data ", VARIN_L(*(std::string*)constVal.v.get()))});
            break;
        }
    }
    return cmdlist;
}
CMD_LIST initializeRegisters(std::vector<std::string> names)
{
    CMD_LIST _Instructions;

    for (int i = 0; i < names.size(); i++)
    {
        char id = names[i][0];
        switch(id)
        {
            // adders
            case 'a':
            {
                _Instructions.push_back({"scoreboard", MC_SCOREBOARD_OBJ_ADD(VARIN(names[i]), dummy, VARIN_L(QUOTE(names[i])))});
                _Instructions.push_back({"scoreboard", MC_SCOREBOARD_PLAYER_ADD("_RSRDC_", VARIN(names[i]), "0")});
                break;
            }
            // everything else is a default storage variable
            default:
            {
                _Instructions.push_back({"data", MC_LIST_APPEND_VALUE("storage", RS_PROGRAM_DATA(/registers), "{data: 0, state: 0}")});
                break;
            }
        }
    }

    return _Instructions;
}
CMD_LIST _rs_initProgram(mcf::rscprogram &prog)
{
    /**
     * data merge storage redscript:program_data {variables: [], scope: 0, functions: ['run'], call_stack: [], instruction: 0, entry_point: 'run'}
     *
     */
    // create program data
    CMD_LIST _InitializationInstructions;

    mcf::command setup{"data", MC_DATA_MERGE("storage", RS_PROGRAM_ROOT, RS_PROGRAM_DEFAULT_DATA(prog._ChosenEntryIndex))};

    _InitializationInstructions.push_back(setup);
    _InitializationInstructions.push_back({"scoreboard", MC_SCOREBOARD_OBJ_ADD("_SCOPE", dummy, "_SCOPE")});
    _InitializationInstructions.push_back({"scoreboard", MC_SCOREBOARD_PLAYER_ADD("_PROGDATA", "_SCOPE", "0")});
    // init registers
    CMD_LIST regs = initializeRegisters({ALL_REGISTER_NAMES});
    _InitializationInstructions.insert(_InitializationInstructions.end(), regs.begin(), regs.end());

    return _InitializationInstructions;
}