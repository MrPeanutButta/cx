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
{{source|if(x)
    int i;
// i is no longer in scope
}}
is the same as
{{source|if(x) {
    int i;
} // i is no longer in scope
}}

The scope of the name introduced by {{spar|condition}}, if it is a declaration, is the same as the scope of the body of the statements:

{{source|1=if (int x = f()) {
    int x; // error: redeclaration of x
}
else {
    int x; // error: redeclaration of x
}
}}


===Keywords===

{{ltt|cpp/keyword/if}},
{{ltt|cpp/keyword/else}}

===Example===
{{example
 | The following example demonstrates several usage cases of the {{tt|if}} statement
 | code=
#include <iostream>

int main()
{
    // simple if-statement with an else clause
    int i = 2;
    if (i > 2) {
        std::cout << i << " is greater than 2\n";
    } else {
        std::cout << i << " is not greater than 2\n";
    }

    // nested if-statement
    int j = 1;
    if (i > 1)
        if(j > 2)
            std::cout << i << " > 1 and " << j << " > 2\n";
        else // this else is part of if(j>2), not part of if(i>1) 
            std::cout << i << " > 1 and " << j << " <= 2\n";

   // declarations can be used as conditions with dynamic_cast
   struct Base {
        virtual ~Base() {}
   };
   struct Derived : Base {
       void df() { std::cout << "df()\n"; }
   };
   Base* bp1 = new Base;
   Base* bp2 = new Derived;

   if(Derived* p = dynamic_cast<Derived*>(bp1)) // cast fails, returns NULL
       p->df();  // not executed

   if(auto p = dynamic_cast<Derived*>(bp2)) // cast succeeds
       p->df();  // executed
}
 | output=
2 is not greater than 2
2 > 1 and 1 <= 2
df()
}}

[[de:cpp/language/if]]
[[es:cpp/language/if]]
[[fr:cpp/language/if]]
[[it:cpp/language/if]]
[[ja:cpp/language/if]]
[[pt:cpp/language/if]]
[[ru:cpp/language/if]]
[[zh:cpp/language/if]]
