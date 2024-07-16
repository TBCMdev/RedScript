# RedScript

## What is RedScript?

Redscript is a programming language that converts high level code into a `.mcfunction` file.
This file can then be executed in a world manually, or on certain events that occur in the world/server.
Redscript opens up programmers to minecraft, allowing them to create complex machines and projects without the need of extensive research over redstone components and especially commands.

## Getting Started

To get started, head over to our website and install the redscript compiler.

## Hello World

Now we can start writing some redscript! First, create a file with the extention `.rsc`. The entry point to the program is in the `run` function. This is called when a player executes the command manually. We must specify that it returns an integer, to inform minecraft of the status of the command and if it was a success or not.
```
int run(args: string[])
{
    // code here...
}
```
**To send a message to someone, we use the `msg` function.**
```
// sends a message to the specified player(s).
void msg(_Specifier: int64, _Args: string[]);
```
Lets send a message to everyone when the command is executed.
```
macro ALL_PLAYERS 2;
int run(args: string[])
{
    msg(ALL_PLAYERS, "Hello World!");
}
```
The msg function needs an integer to specify who to send it to. You are allowed to pass 2 into the function instead of creating a macro, but the whole point of the macro is to make the code more readable.

## Variables
Variables in **RedScript** are quite similar to other programming languages in terms of syntax, yet their functionality behind the hood is quite different.

To declare a variable, the name goes first, followed by its mandatory type specifier.

```
myint: int = 44;
```

These variables are stored in the scoreboard mechanic in minecraft.
That is, when you create a variable, the compiler recognizes this and adds its name to the scoreboard. 
When the variable goes out of scope or a variable is manually deleted with the `delete` keyword, the variable is removed from the scoreboard entirely.

**It is important to note that** your variables are stored in the scoreboard on a **Fake Player**, meaning they are not player dependent. To make player dependent variables, we have to take a different approach. We need to use the scoreboard module.

```
use scoreboard_dll; // every file included needs '_dll'! This is explained in a later chapter.


int run(args: string[])
{
    ScoreboardField field = scoreboardAdd("mytestvariable", 0);

    updateFieldValue(field, 44); // updates for all players
    updateFieldValueFor(field, "John", 45) // updates only for 'John' (His username).

}
```

## Notes on the Scoreboard

As the scoreboard is one of the only reliable and fast ways we can store variables in minecraft, program execution variables are also stored there. Such as scope, program counter, object jsons, etc. Even though they are hard to access, changing them manually can have tragic consequences if you don't know what you are doing. Modifying the excection of the program could lead to your game crashing.

## Objects

Objects in Minecraft are in JSON format. Therefore, in this version of RedScript, you cannot run functions on objects, you can only access variables.
Raw JSON is not currently supported in RedScipt as opposed to a language like JavaScript, however that's where objects come in.

Objects abstract the JSON structure, and can be used to store large amounts of data with ease. 

**NOTE: you can still use functions with objects, they just have to be implemented like c, the functions are external to the object schema and are passed in an instance of the object as a parameter.**

**NOTE: Object inheritance is not yet supported.**

Here is an example of a Player object.

```
use uuid_dll;
object Player
{
    uuid: UUID;
    name: string;
    health: float;
    max_health: float;
    is_host: bool;
}
```

A UUID is also another object in the internal library, showing the abstractional power these objects hold. You can access fields with the `.` operator:
```
myPlayer.health // -> 20
```
### Constructors

Constructors are a way of creating instances of an object. However they are not yet supported in the early stages of RedScript. We can get around it by using an external function to create it for us.

```
unknown createObject(schema: object, items: void...);
```

And we can use `createObject` as shown below:

```
Player myPlayer = createObject(Player, 'UUID-here', 'RedScripter', 20, 20, false);
```
It is important to note that parameters passed into the items void array can be null by using the `NULL` keyword, but they must be in order. 