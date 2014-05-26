#include io
#include char

/** @param void
 */
int main () {

	io::file in;

	if (!in.open("test.txt", "r")){
		io::perror("open failed");
		return 0;
	}

	int numbers = 0;
	int chars = 0;
	int punct = 0;
	int hex_num = 0;

	char c;

	while (!in.eof){
		c = in.getc;

		if (c.is_digit) numbers++;
		if (c.is_alpha) chars++;
		if (c.is_punct) punct++;
		if (c.is_xdigit) hex_num++;
	}

	in.close;

	io::puts("numbers: " + numbers.to_str);
	io::puts("chars: " + chars.to_str);
	io::puts("punct: " + punct.to_str);
	io::puts("hex_num: " + hex_num.to_str);
    
    return 0;
}