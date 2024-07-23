#include <string>
#include "mcf.hpp"


// mcf mappings

#define CXPR constexpr 
#define MAPPING(x) CXPR std::vector<std::string> m_##x(std::vector<voidnode>);
#define INTERNAL_MAPPING(x) CXPR std::vector<std::string _inb_m_##x(std::vector<voidnode>);
// need function to evaluate void node, store result in register,
// and then return that register at compile time to all mappings.
CXPR mcf_register evalVoidNode(voidnode&);

MAPPING(exitProgram);
MAPPING(createLocalVariable);
MAPPING(createGlobalVariable);
MAPPING(callFunction);
MAPPING(deleteVariable);
MAPPING(getRegister);
MAPPING(setRegister);
MAPPING(runRawCommand);
MAPPING(add);MAPPING(sub);MAPPING(mul);MAPPING(div);MAPPING(mod);
MAPPING(adds);MAPPING(subs);MAPPING(muls);MAPPING(divs);MAPPING(mods);
MAPPING(delay);



