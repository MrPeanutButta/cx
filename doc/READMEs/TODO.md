#TODO

All TODO items will be tracked here.

## Assignment Operators
Assignment, including compound assignment operators are currently being refactored in assign_ops.cpp.

## IO
File streams are currently being refactored in io.cpp.

## Data Types and Constants
Need to complete:

* void : function returning nothing
* null : constant
* auto : dynamic type
    + a fully dynamic type like in ruby
* multidimentional arrays
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

## Function Overloading
This needs to be planned. No code has been written to address this yet.