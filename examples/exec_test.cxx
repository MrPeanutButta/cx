#include io
#include char

class t : char {
    int p(){
        io::puts("test called");
        
        return 0;
    }
};

/** @param void
 */
int main() {

    io::file in;

    t test;

    test.p;
    /*if (!in.open("test.txt", "r")) {
        io::perror("open failed");
        return 0;
    }

    int numbers = 0;
    int chars = 0;
    int punct = 0;
    int hex_num = 0;

    char *c = in.readfile();
    in.close;
    
    io::puts(test.is_digit.to_str);
    
    for (int i = 0; i < c.length; i++) {
        if (c[i].is_digit) numbers++;
        if (c[i].is_alpha) chars++;
        if (c[i].is_punct) punct++;
        if (c[i].is_xdigit) hex_num++;	

        io::puts(c[i].to_upper.to_str);
    }

    io::puts("numbers: " + numbers.to_str);
    io::puts("chars: " + chars.to_str);
    io::puts("punct: " + punct.to_str);
    io::puts("hex_num: " + hex_num.to_str);
    
    io::puts("total char * length = " + c.length.to_str);*/

    return 0;
}