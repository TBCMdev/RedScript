#pragma once
#include <string>

#define RS_PROGRAM_ROOT "redscript:program_data"
#define RS_PROGRAM_DATA(W) RS_PROGRAM_ROOT #W
#define RS_PROGRAM_DEFAULT_DATA(_Entry) "{variables: {}, scope: 0, functions: [], call_stack: [], instruction: 0, entry_point: '" + std::to_string(_Entry) + "'}"
#define RS_PROGRAM_ROOT_STORAGE MCSTORAGE(RS_PROGRAM_ROOT)

#define RS_VARIABLE_JSON(_var, _scope) "{\"name\":\"" + _var.name + "\",\"scope\":" + std::to_string(_scope) + "\",\"value\": 0,\"type\":\"" + std::to_string(_var.type._TypeID) + "\"}"
#define RS_VARIABLE_JSON_VALUE(_var, _value, _scope) "{\"name\":\"" _var.name "\",\"scope\":" + std::to_string(_scope) + "\",\"value\": " + std::to_string(_value) + ",\"type\":\"" + std::to_string(_var.type._TypeID) + "\"}"

#pragma region data

#ifdef RS_KEYWORDS 
#define STORAGE storage
#define MCSTORAGE(_where) "storage",_where
#define ENTITY entity
#define MCENTITY(_source) "entity",_source
#define BLOCK block
#define MCBLOCK(x,y,z) "block",#x #y #z

#define VALUE value
#define STRING string
#define FROM from
#endif 


// always defined. Use if you want to leave a parameter in the following macros blank.
#define MNULL ""
#define SP " "
#define VARIN(V) + (V) +
#define VARIN_L(V) + (V)
#define QUOTE(X) "'" + X + "'"

#pragma region data_get

#define CMD_DATA_GET(_type, _target, _where, _scale) ("get " SP _type SP _target SP _where SP _scale)

#pragma endregion data_get

#pragma region data_remove

#define CMD_DATA_REMOVE(_type, _target, _where) ("remove " _type SP _target SP _where)

#pragma endregion data_remove

#pragma region data_modify
#define CMD_DATA_MODIFY(_type, _target, _operation, _where, ...) ("modify " _type SP _target SP #_operation SP _where SP #__VA_ARGS__)

#define MC_LIST_APPEND_FROM(_type, _target, _source, _fpath) CMD_DATA_MODIFY(_type, _target, append SP from, _source, _fpath)
#define MC_LIST_APPEND_STRING(_type, _target, _source, _spath, _from, _to) CMD_DATA_MODIFY(_type, _target, append SP string, _source, _spath, _from, _to)
#define MC_LIST_APPEND_VALUE(_type, _target, _value) CMD_DATA_MODIFY(_type, _target, append value, _value)

#define MC_LIST_PREPEND_FROM(_type, _target, _source, _fpath) CMD_DATA_MODIFY(_type, _target, prepend SP from, _source, _fpath)
#define MC_LIST_PREPEND_STRING(_type, _target, _source, _spath, _from, _to) CMD_DATA_MODIFY(_type, _target, prepend SP string, _source, _spath, _from, _to)
#define MC_LIST_PREPEND_VALUE(_type, _target, _value) CMD_DATA_MODIFY(_type, _target, prepend SP value, _value)

#define MC_LIST_INSERT_FROM(_type, _target, _source, _fpath, _index) CMD_DATA_MODIFY(_type, _target, insert SP #_index SP from, _source, _fpath)
#define MC_LIST_INSERT_STRING(_type, _target, _source, _spath, _index, _from, _to) CMD_DATA_MODIFY(_type, _target, insert SP #_index SP string, _source, _spath, _from, _to)
#define MC_LIST_INSERT_VALUE(_type, _target, _source, _fpath, _index) CMD_DATA_MODIFY(_type, _target, insert SP #_index SP value, _source, _fpath)

#define MC_DATA_SET_FROM(_type, _target, _source) CMD_DATA_MODIFY(_type, _target, set SP from, _source)
#define MC_DATA_SET_STRING(_type, _target, _source, _spath, _from, _to) CMD_DATA_MODIFY(_type, _target, set SP string, _source, _spath, _from, _to)
#define MC_DATA_SET_VALUE(_type, _target, _source, _value) CMD_DATA_MODIFY(_type, _target, _source, set SP value, _value)
#pragma endregion data_modify

#pragma region data_merge

#define MC_DATA_MERGE(_type, _target, _nbt) ("merge " _type SP _target SP _nbt)

#define MC_MERGE_FROM(_type, _target, _source) CMD_DATA_MERGE(_type SP _target, _source)
#define MC_MERGE_STRING(_type, _target, _source, _from, _to) CMD_DATA_MERGE(_type SP _target, _source) SP #_from SP #_to
#define MC_MERGE_VALUE(_value) ("merge value " SP _value)

#pragma endregion

#pragma region scoreboard

#define CMD_SCOREBOARD_OBJ(_operation, _name, _type, _displayName) "objectives " #_operation SP _name SP #_type SP _displayName
#define CMD_SCOREBOARD_PLAYER(_operation, _targets, _objective, _score) "players " #_operation SP _targets SP _objective SP #_score

#define MC_SCOREBOARD_OBJ_ADD(_name, _type, _displayName) CMD_SCOREBOARD_OBJ(add, _name, _type, _displayName)
#define MC_SCOREBOARD_OBJ_SETDISPLAY(_display, _objective) "objectives setdisplay " #_display SP _objective
#define MC_SCOREBOARD_OBJ_REMOVE(_name) "objectives remove " #_name
#define MC_SCOREBOARD_OBJ_LIST "objectives list"
// TODO
#define MC_SCOREBOARD_OBJ_MODIFY MNULL

#define MC_SCOREBOARD_PLAYER_ADD(_selector, _objective, _score) CMD_SCOREBOARD_PLAYER(add, _selector, _objective, _score)
#define MC_SCOREBOARD_PLAYER_ENABLE(_selector, _objective) CMD_SCOREBOARD_PLAYER(enable, _selector, _objective)
#define MC_SCOREBOARD_PLAYER_DISABLE(_selector, _objective) CMD_SCOREBOARD_PLAYER(disable, _selector, _objective)
#define MC_SCOREBOARD_PLAYER_SET(_selector, _objective, _value) CMD_SCOREBOARD_PLAYER(set, _selector, _objective, _value)
#define MC_SCOREBOARD_PLAYER_RESET(_selector, _objective) CMD_SCOREBOARD_PLAYER_RESET(reset, _selector, _objective)
#define MC_SCOREBOARD_PLAYER_REMOVE(_selector, _objective) "players remove " #_selector #_objective
#define MC_SCOREBOARD_PLAYERS_LIST "players list"

// scoreboard players operation is how we do arithmatic.
// TODO!!
#define MC_SCOREBOARD_OPERATION(_selector, _objective, _operation, _source, _sobjective) "players operation " _selector SP _objective SP _operation SP _source SP _sobjective 


#pragma endregion

#pragma region execute
#define CMD_EXECUTE_AS(_entity, _command) ("execute as " #_entity " run " #_command)
#define CMD_EXECUTE_AT(_entity, _command) ("execute at " #_entity " run " #_command)
#define CMD_EXECUTE_IF(_condition, _command) ("execute if " #_condition " run " #_command)
#define CMD_EXECUTE_UNLESS(_condition, _command) ("execute unless " #_condition " run " #_command)
#pragma endregion execute

#pragma region tp
#define CMD_TP(_entity, _location) ("tp " #_entity SP #_location)
#define CMD_TP_TO_ENTITY(_entity, _target) ("tp " #_entity " to " #_target)
#define CMD_TP_COORDINATES(_entity, _x, _y, _z) ("tp " #_entity SP #_x SP #_y SP #_z)
#pragma endregion tp

#pragma region summon
#define CMD_SUMMON(_entity, _x, _y, _z) ("summon " #_entity SP #_x SP #_y SP #_z)
#pragma endregion summon

#pragma region setblock
#define CMD_SETBLOCK(_x, _y, _z, _block) ("setblock " #_x SP  #_y SP  #_z SP  #_block)
#define CMD_SETBLOCK_NBT(_x, _y, _z, _block, _nbt) ("setblock " #_x SP  #_y SP  #_z SP  #_block SP  #_nbt)
#pragma endregion setblock

#pragma region give
#define CMD_GIVE(_player, _item, _count) ("give " #_player SP #_item SP #_count)
#pragma endregion give

#pragma region clear
#define CMD_CLEAR(_player, _item, _count) ("clear " #_player SP #_item SP #_count)
#pragma endregion clear

#pragma region kill
#define CMD_KILL(_target) ("kill " #_target)
#pragma endregion kill

#pragma endregion data



using mc_cmd = const char*;
