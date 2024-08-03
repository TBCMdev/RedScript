#version 1.21

int tellraw(selector__: string, message__: string);

int run(args: string[])
{
    str: string = "Hello!";

    tellraw(@p, str);
}