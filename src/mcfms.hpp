#pragma once

// mcf mappings
// all needed mappings

#include <string>
#include <unordered_map>
#include <vector>

#define RS_KEYWORDS 1

#include "commands.hpp"
#include "mcf.hpp"
#define CMD_LIST std::vector<mcf::command>

std::pair<CMD_LIST, mcf::mcf_register*> evalVoidnode(voidnode&);


// exitProgram;
// createLocalVariable;
CMD_LIST initializeRegisters(std::vector<std::string>);
CMD_LIST _rs_initProgram(mcf::rscprogram&);

CMD_LIST createLocalVariable(mcf::rs_variable&);
CMD_LIST createLocalVariable(mcf::rs_variable&, int);
CMD_LIST assignVariableValue(std::string, voidnode &);
CMD_LIST assignVariableValue(std::string, shared_tvoidp&);

std::pair<CMD_LIST, mcf::mcf_register*> getRegister(mcf::mcf_register&);
CMD_LIST setRegister(mcf::mcf_register&, shared_tvoidp);

std::pair<CMD_LIST, mcf::mcf_register*> _ARRoperate(mcf::mcf_register&, shared_tvoidp&, std::string);
std::pair<CMD_LIST, mcf::mcf_register*> _ARRoperate(shared_tvoidp&, shared_tvoidp&, std::string);
std::pair<CMD_LIST, mcf::mcf_register*> _AARoperate(mcf::rs_variable&, tvoidp&, bool, bool, std::string);


CMD_LIST prepareFunctionCall(mcf::function_byte_code&, std::vector<shared_tvoidp>&, std::string*);

#define ARR_ADD(tvp1, tvp2) _ARRoperate(tvp1, tvp2, "+=")
#define ARR_SUB(tvp1, tvp2) _ARRoperate(tvp1, tvp2, "-=")
#define ARR_MUL(tvp1, tvp2) _ARRoperate(tvp1, tvp2, "*=")
#define ARR_DIV(tvp1, tvp2) _ARRoperate(tvp1, tvp2, "/=")
#define ARR_MOD(tvp1, tvp2) _ARRoperate(tvp1, tvp2, "%=")
// createGlobalVariable;
// callFunction;
// deleteVariable; 
// runRawCommand;
// add;sub;mul;div;mod;
// adds;subs;muls;divs;mods;



