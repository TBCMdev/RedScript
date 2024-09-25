#version 1.21

use events;
use world;
use player;
use math;

int PlayerJumpEvent(player: Player) override
{
    r: int = rand(0, 10);

    if(r == 0)
    {
        d: int = rand(0, 20);
        damagePlayer(player, d);
    }
}