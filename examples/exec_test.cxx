#include io

/** @param void
 */
int main () {

    //bool *b = {true,false,false,true,true};
    //int *a = {0,1,2,3,4};
    wchar *c = "some long ass string to_str splat";
   // float *d = {1.1,2.2,3.3,4.4,5.5};
        
    /*io::puts(a.to_str);
    io::puts(b.to_str);*/
    io::putws(c.to_wstr);
    //io::puts(d.to_str);
    
    return 0;
}
