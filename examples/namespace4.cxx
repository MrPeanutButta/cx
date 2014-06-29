#include io
#include char

namespace ding {
    
    int a;
    int b;
    char c;
    
    int t(int &i) {
        return (i * i);
    }
    
    void tt(char c){
        io::puts("ding::tt(" + c.to_str + ") called");
    }
}

/** @param void
 */
int main () {

    char c = 'P';
    ding::c = 'A';
    
    ding::a = 10;
    bool a = false;
    bool b = true;
   
    ding::tt(c);
    
    int p = ding::t(ding::a);
	
    io::puts(p.to_str);
    io::puts(ding::t(ding::a).to_str);
    io::puts(ding::c.is_xdigit.to_str);
    io::puts(c.is_xdigit.to_str);

    return 0;
}
