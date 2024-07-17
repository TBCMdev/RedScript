#version 1.21

macro EXECUTOR 1;

int run(args: string[])
{
    exec("kill", EXECUTOR);
    msg(EXECUTOR, "You shouldn't have run this command...");
}