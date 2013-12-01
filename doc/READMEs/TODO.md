#TODO

All TODO items will be tracked here.

## Assignment Operators
Assignment, including compound assignment operators are currently being refactored in assign.cpp.

## Expressions
Need to refactor expressions for all data types.

## Reference Operator &
Variable take the address of right hand side operand.

## IO
File streams are currently being refactored in io.cpp.

## Data Types and Constants (active)
Need to complete:

* assignment based on byte size and type compatibility
* void : function returning nothing
* null : constant null value
* const : needs work
* enum : needs work
* auto : dynamic type
    + a fully dynamic type like in ruby
* multi-dimentional arrays
* initialization lists {}
* pass by reference
* reference to var or objects
* class : basic objects
    + classes will copy inherit all contents with associated scopes.
 
``` cpp
class object : base{
public:

    object(){}  // calls base ctors
    ~object(){} // calls base dtors

    int var1;

protected:
    int var2;
private:
    int var3;
};
```

* namespace : icode sections to prevent name collisions

``` cpp
namespace std{
    int var1;
    int var2;

    void function(){
        return;
    }
};
```
## Conditions
All conditions should be refactored as boolean instead of int.

## Function Overloading
This needs to be planned. No code has been written to address this yet.

## Documentation
Operator documentation is mostly complete. Need to document things as they are developed.

## Lexer/Scanner
Needs to recognize '\000' as an unsigned char.

## Entry-point Parameters
main needs parameters.

## Runtime Stack and GC
Need to implement an iterator as the TOS pointer. This will enable passing by refence.
This will bring forth the need for garbage collection as well.