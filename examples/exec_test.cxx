#include io
#include char

/** @param void
 */
int main () {

    char *c = "s7o5me lvbvvon1g asbs st5rdding taao_s6tr spvvf0lat";
    char *result;
    
    for (int i = 0; i < c.length; i++)
        if (c[i].is_digit)
            result += c[i];
    
    int a = result.to_int * 2;
    
    io::puts(a.to_str);
    
    return 0;
}