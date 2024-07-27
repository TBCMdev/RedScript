#include "mcfms.hpp"

#pragma region maths

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

            CMD_LIST setup_r1 = setRegister(*regs.first, *(int *)l.v.get());
            CMD_LIST setup_r2 = setRegister(*regs.second, *(int *)r.v.get());

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

    if (vn._Left->index() == 1)
    {
        auto l = std::get<1>(*vn._Left);
        auto ret = evalVoidnode(l);

        _Instructions.insert(_Instructions.end(), ret.first.begin(), ret.first.end());
        
        if (vn._Right->index() != 1)
        {
            shared_tvoidp& r = std::get<0>(*vn._Right);

            auto l_add_r = _ARRoperate(l.v, r, bst_operator_str(vn._Operator));
        }
    }
    else
    {
        shared_tvoidp& l = std::get<0>(*vn._Left);
        if(l.t != VARIABLE_TYPE_ID && vn._Right->index() == 0)
        {   
            shared_tvoidp& r = std::get<0>(*vn._Right);
            if(r.t == l.t)
            {
                // they are both raw nodes with just numbers, we can add them at compile time
                switch(vn._Operator)
                {
                    case bst_operator::ADD: l.v = std::make_shared<void>((*(int*)l.v.get()) + (*(int*)r.v.get())); break;
                    case bst_operator::SUB: l.v = std::make_shared<void>((*(int*)l.v.get()) - (*(int*)r.v.get())); break;
                    case bst_operator::MUL: l.v = std::make_shared<void>((*(int*)l.v.get()) * (*(int*)r.v.get())); break;
                    case bst_operator::DIV: l.v = std::make_shared<void>((*(int*)l.v.get()) / (*(int*)r.v.get())); break;
                    case bst_operator::MOD: l.v = std::make_shared<void>((*(int*)l.v.get()) % (*(int*)r.v.get())); break;
                }
                // invalid number now, it has been operated on.
                r.v.reset();
            }

        }
    }

    if (vn._Right->index() == 1)
    {
        auto ret = evalVoidnode(std::get<1>(*vn._Right));
        _Instructions.insert(_Instructions.end(), ret.first.begin(), ret.first.end());
    }
}

CMD_LIST createLocalVariable(mcf::rs_variable &variable, int _scope)
{
    return {mcf::command{"data", MC_DATA_MERGE(RS_PROGRAM_DATA(/variables), RS_VARIABLE_JSON(variable, _scope))}};
}
CMD_LIST createLocalVariable(mcf::rs_variable &variable, voidnode &value, int _scope)
{
}
CMD_LIST initializeRegisters(std::vector<std::string> names)
{
    CMD_LIST _Instructions;

    for (int i = 0; i < names.size(); i++)
    {
        _Instructions.push_back({"scoreboard", MC_SCOREBOARD_OBJ_ADD(VARIN(names[i]), dummy, VARIN_L(QUOTE(names[i])))});
        _Instructions.push_back({"scoreboard", MC_SCOREBOARD_PLAYER_ADD("_RSRDC_", VARIN(names[i]), 0)});
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

    mcf::command setup{"data", MC_DATA_MERGE(RS_PROGRAM_ROOT_STORAGE, RS_PROGRAM_DEFAULT_DATA(prog._ChosenEntryIndex))};

    _InitializationInstructions.push_back(setup);

    // init registers
    CMD_LIST regs = initializeRegisters({ALL_REGISTER_NAMES});
    _InitializationInstructions.insert(_InitializationInstructions.end(), regs.begin(), regs.end());

    return _InitializationInstructions;
}