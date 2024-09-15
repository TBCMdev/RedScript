
const int onPlayerDeath(__player: Player) virtual;
const int onPlayerKillPlayer(__killer: Player, __killed: Player) virtual;
const int onPlayerKill(__player: Player) virtual;
const int onPlayerHealthChange(__player: Player, __nhealth: double) virtual;
const int onPlayerXPChange(__player: Player, __nxp: double) virtual;
const int onPlayerHungerChange(__player: Player, __nh: double) virtual;
const int onPlayerBreathChange(__player: Player, __nair: double) virtual;
const int onPlayerArmorChange(__player: Player, __narmor: double) virtual;

const int onPlayerJump(__player: Player) virtual;


