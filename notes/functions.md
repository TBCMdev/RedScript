# FUNCTIONS (3/8/2024)

## MACROS
mcfunction files actually have macros, allowing us to pass variables from data storage or static constants into command parameters.

```
# preceed line with $, all parameters have $ preceeded too
$say $p1
```

We can use the with keyword to pass data:

```
run function with storage ... <with> # do we chain the withs here? idk
```

One problem, if we pass a variable's value, and we change it in the function, i dont think the macro will update.
We must hard code a solution that follows variable updates.


## RETURN

The return command is how we can terminate function execution, something ive been scratching my head on for a while, adde 1.20.
