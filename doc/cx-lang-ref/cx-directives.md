---
layout: page
title: "Cx-directives"
date: 2014-31-01 09:55
comments: true
sharing: true
footer: true
---

All Cx directives start with the pound `#` operator

`#include filename`
Creates a separate parser instance and opens the file for processing. Symbols loaded from the header file will be available within the current scope. This directive will first search for the environment path variable `CX_STDLIB`, then search the local directory for the header module.<br>
``` cpp
#include stdio
```


`#warn "string message"`
Prints warning to stderr and continues with parsing the code module.