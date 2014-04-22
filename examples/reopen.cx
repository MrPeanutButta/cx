#include io

int main () {

	io::file outfile;

	if(!outfile.open("textfile", "w"))
		io::puts("error opening file");
	else {

		outfile.puts("example put string, write to file");
		outfile.close();
		
		if(!outfile.reopen("textfile2", "w"))
			io::puts("error reopening file");
		else {
			outfile.puts("example put string, reopen write to file");
			outfile.close();
		}
	}

    return 0;
}