# if
Conditionally executes code.
Used where code needs to be executed only if some condition is present.

### Syntax
-
**if** **(** *condition* **)** *statement_true*

-
**if** **(** *condition* **)** *statement_true* **else** *statement_false*

-
* *condition* - any expression which is contextually convertible to bool.
* *statement_true* - any statement often a compound statement, which is executed if *condition* evaluates to **true**.
* statement_false - any statement often a compound statement, which is executed if *condition* evaluates to **false**

### Explanation

If the *condition* yields **true**, *statement_true* is executed.

If the else part of the if statement is present and condition yields *false*, *statement_false* is executed.

In the second form of if statement (the one including else), if {{spar|statement_true}} is  also an if statement then that inner if statement must contain an else part as well (in other words, in nested if-statements, the else is associated with the closest if that doesn't have an else)

### Notes
If *statement_true* or *statement_false* is not a compound statement, it is treated as if it was:
``` cpp
if(x)
    int i;
// i is no longer in scope

// is the same as
if(x) {
    int i;
} // i is no longer in scope
```
The scope of the name introduced by *condition*, if it is a declaration, is the same as the scope of the body of the statements:

``` cpp
if (int x = f()) {
    int x; // error: redeclaration of x
}
else {
    int x; // error: redeclaration of x
}
```


### Keywords

`if`
`else`

### Example
The following example demonstrates several usage cases of the `if` statement
``` cpp 
#include io

int main(){
    // simple if-statement with an else clause
    int i = 2;
    if (i > 2) {
        io::puts(i.to_str + " is greater than 2\n");
    } else {
        io::puts(i.to_str +  " is not greater than 2\n");
    }

    // nested if-statement
    int j = 1;
    if (i > 1)
        if(j > 2)
            io::puts(i.to_str + " > 1 and " + j.to_str + " > 2\n");
        else // this else is part of if(j>2), not part of if(i>1) 
            io::puts(i.to_str + " > 1 and " + j.to_str + " <= 2\n");

}
```

#### output
```
2 is not greater than 2
2 > 1 and 1 <= 2
```