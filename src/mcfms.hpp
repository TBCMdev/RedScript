#pragma once

#include <string>
#include <unordered_map>

#include "mcf.hpp"
// mcf mappings
#define MAPPING_SIG std::vector<std::string>(*)(std::vector<voidnode>)
#define MAPPING_SIGF(F) std::vector<std::string>(*F)(std::vector<voidnode>)
#define FUNCTORCOLLECTION static inline std::unordered_map<const char*, MAPPING_SIG>

FUNCTORCOLLECTION _rs_Mappings;

constexpr void registerMapping(const char* name, MAPPING_SIGF(func)) {
    _rs_Mappings[name] = func;
}
#define CXPR constexpr 

#define MAPPING(x) auto m_##x(std::vector<voidnode>) -> std::vector<std::string>
#define MDEF(fName) \
    inline constexpr bool fName##_registered = (registerMapping(#fName, &m_##fName), true); \
    MAPPING(fName)
#define INTERNAL_MAPPING(x) CXPR std::vector<std::string> _inb_m_##x(std::vector<voidnode>)

CXPR mcf::mcf_register evalVoidNode(voidnode&);

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



