#include io

int main () {

    bool b = true;

    namespace b {
        bool is_false(){		
            return (this == false);
    	}
    }
   
    io::puts(b.is_false.to_str);

    return 0;
}