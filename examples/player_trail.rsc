#version 1.21

use events; # PlayerMoveEvent
use world; # blockAt
use player; # Player
use math; # Vec3

macro EMERALD_BLOCK_ID -1;

int PlayerMoveEvent(player: Player) override
{
    bpos: Vec3 = player.pos;
    bpos.y -= 1;
    block: Block = blockAt(bpos, player.dimension);
    
    setBlockType(block, EMERALD_BLOCK_ID); 
}