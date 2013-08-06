This document is to serve as the standard coding convention reference for Cx.
At any time you come across anything undocumented, just be aware of what the
already existing code looks like and follow it's format.

In the event you find code not following the standard, please fix it if it
will not cause much of an issue. If the non-standard code requires much work,
submit an issue and someone will help correct it.

BASICS

  Limit lines to 79 characters.

  Use form feeds (control+L) to divide long source files into logical
  pieces.  A form feed should appear as the only character on a line.

  Do not use tabs for indentation.

  Avoid trailing spaces on lines.

NAMING - needs to be done.

  Use names that explain the purpose of a function or object.

  Use underscores to separate words in an identifier: multi_word_name. 

  Use lowercase for most names.  Use uppercase for macros, macro
parameters, and members of enumerations.

  Give arrays names that are plural.

  Pick a unique name prefix (ending with an underscore) for each
module, and apply that prefix to all of that module's externally
visible names.  Names of macro parameters, struct and union members,
and parameters in function prototypes are not considered externally
visible for this purpose.

  Do not use names that begin with _.  If you need a name for
"internal use only", use __ as a suffix instead of a prefix.

  Avoid negative names: "found" is a better name than "not_found".

  In names, a "size" is a count of bytes, a "length" is a count of
characters.  A buffer has size, but a string has length.  The length
of a string does not include the null terminator, but the size of the
buffer that contains the string does.

Comments:
    single line comments should only use C++ one line comments.
    comment blocks should use C style comments to denote that these comments
    are grouped.

    // single liner not documented

    /* we have lots to talk about
     * here so we use
     * comment blocks for non-doxygen. */

     Doxygen Comments:
     /// for one liners
     /** main_topic     explanation.
      *
      * NOTE:
      *     notes go here.
      */

Pointers:
    must always be initialized with nullptr if nothing else.
    must begin with prefix 'p_'.

    example:
        char *p_file_name;

NULL vs nullptr:
    always use nullptr.
   