execute if score _PROGDATA _FDEPTH > 65536 run return 0
execute if score _PROGDATA _FOR >= $(_until) run return 0

$(_exec)

$scoreboard players $(_operation) _PROGDATA _FOR 1

scoreboard players add _PROGDATA _FOR 1
$function redscript/for {_operation: $(operation), _until: $(_until), _exec: $(_exec)}
