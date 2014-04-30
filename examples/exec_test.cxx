#include io

/** @param void
 */
int main () {

    //char *a_string = "100";
    bool *b = {true,false,false,true,true};
    int *a = {0,1,2,3,4};
    char *c = "some long ass string to_str splat";
    float *d = {1.1,2.2,3.3,4.4,5.5};
    
    //int a_int = ;
    
    io::puts(a.to_str);
    io::puts(b.to_str);
    io::puts(c.to_str);
    io::puts(d.to_str);
    
    return 0;
}
