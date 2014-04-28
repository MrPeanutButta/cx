#include io
#include char

class bool {
    bool is_false() {		
        return (this == false);
    }
}

class char {
    char do_di_do(int a) {
        return (this + a);
    }
}

namespace ding {
    void t(char &ch) {
        io::puts("ding::t(" + ch.to_str + ") called");
    }
}

/** @param void
 */
int main () {

    char c = 'F';
    bool a = false;
    bool b = true;
   
    ding::t(c);
	
    io::puts(c.do_di_do(10).to_str);
    io::puts(a.is_false.to_str);
    io::puts(b.is_false.to_str);

    return 0;
}