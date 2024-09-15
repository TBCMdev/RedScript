#version 1.21

string my_test_function_that_exists_in_other_datapack(s: string);


int run(args: string[])
{
    my_test_function_that_exists_in_other_datapack("Hello!");
}
