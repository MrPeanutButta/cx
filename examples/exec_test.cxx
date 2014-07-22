#include io
#include char 
#include std

void test_std(){
	io::puts("TEST std");
	io::puts("getenv: " + std::getenv("PATH"));
	io::puts("system: " + std::system("dir").to_str);
	
	std::srand(10);
	io::puts("rand: " + std::rand.to_str);
	return;
}

/** @param void
 */
int main() {
	test_std;

    //system("ls");
    io::file in;
	io::file out;

    if (!in.open("C:\\Users\\aahebert\\Documents\\GitHub\\cx\\doc\\draft\\extending.md", "r")) {
        io::perror("open failed");
        return 0;
    }

	if (!out.open("output.txt", "w")){
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

        //io::puts(c[i].to_upper.to_str);
		out.puts(c[i].to_str);
    }

	out.close;

    io::puts("numbers: " + numbers.to_str);
    io::puts("chars: " + chars.to_str);
    io::puts("punct: " + punct.to_str);
    io::puts("hex_num: " + hex_num.to_str);
    
    io::puts("total char * length = " + c.length.to_str);

    return 0;
}