use schematics_dll;

int build()
{
    buildSchematic(loadSchematic("binary_adder.schem"), 0, 0, 0);
}