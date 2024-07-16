# Building with RedScript

Redscript is a language that communicates with all aspects of minecraft, especially building. To interact with the world around us, we will include the `world` package.
```
use world_dll;
```
**NOTE: The documentation for these packages can be found on our website.**


## Placing Blocks
To place a block, we use the `place` function.

```
void place(block: string, x:int, y:int, z:int);
void place(block: string, where: vec3); // vec3 is an object comprising of x y and z components. It is found in math_dll.
```
Based on the version of minecraft, the block id will be different. For your version, you can define your version at the top of your file using a macro definition:
```
#version "1.21"
```
If you don't define what version you are using, the compiler will issue a warning but will choose the latest supported version.

For most popular versions, macros have been created for each block. For example:
```
#macro ACACIA_WOOD_SLAB "126:4"
```
And it is worth noting that you can use minecraft tags as well here. 
```
place("minecraft:acacia_wood_slab", 0, 2, 0);
```