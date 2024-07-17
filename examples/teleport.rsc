#version 1.21

use player_dll;

before: vec3 = null;

macro EXECUTOR 1;

int run(args: string[])
{
    before = getLocation(EXECUTOR);
    v:vec3 = createObject(vec3, int(args[0]), int(args[1]), int(args[2]));

    schedule(back, 5);

}
null back()
{
    teleport(EXECUTOR, before);
}