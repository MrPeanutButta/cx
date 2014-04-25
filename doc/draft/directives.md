All Cx directives start with the pound `#` operator

`#include filename`
Creates a separate parser instance and opens the file for processing. Symbols loaded from the header file will be available within the current scope. This directive will first search for the environment path variable `CX_STDLIB`, then search the local directory for the header module.<br>
``` cpp
#include stdio
```

`#warn "string message"`
Prints warning to stderr and continues with parsing the code module.

`#import libname`
Searches **CX_STDLIB** first then **PWD** for **libname** to load as a dynamic library. This directive will add the appropriate extension for the current platform, e.g. (.so for Linux, .dll for Windows). There's no need to use the file extension when loading compiled dynamic libraries.

``` cpp
#import stdio // loads stdio.dll or stdio.so
```