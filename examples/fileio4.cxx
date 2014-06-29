#include io
#include char

/** @param void
 */
int main() {

    io::file in;

    if (!in.open("test.txt", "r")) {
        io::perror("open failed");
        return 0;
    }

    int numbers = 0;
    int chars = 0;
    int punct = 0;
    int hex_num = 0;

    char *c = in.readfile;
    in.close;
    
    for (int i = 0; i < c.length; i++) {
        if (c[i].is_digit) numbers++;
        if (c[i].is_alpha) chars++;
        if (c[i].is_punct) punct++;
        if (c[i].is_xdigit) hex_num++;
    }
    
    io::puts(c);

    io::puts("numbers: " + numbers.to_str);
    io::puts("chars: " + chars.to_str);
    io::puts("punct: " + punct.to_str);
    io::puts("hex_num: " + hex_num.to_str);

    return 0;
}