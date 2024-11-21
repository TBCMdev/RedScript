#version 1.21
#use lang

int add(x: int, y: int)
{
    return x + y;
}


int run(args: string[])
{
    z: int = add(4, 5);

    msg(@p, z);
}