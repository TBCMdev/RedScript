#version 1.21

int tellraw(selector__: string, message__: string);

int run(args: string[])
{
    if("Hello" == "Hello World!")
    {
        tellraw(@p, "Comparison is a success");
    }
}