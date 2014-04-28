/* Cx file IO test and example
 *
 * All file IO is transposed from the standard IO library in C.
 * aaron.hebert@gmail.com
 */

 /* still working out the details of importing symbols.
  * #include works for basic header includes */

#include io

bool redir_out(char *filename){
	io::stdout.close;
	return io::stdout.reopen(filename, "w");
}

char *read_file(file &infile){
	char *str = "read: ";

	// while not end of file stream
	while(!infile.eof){
		// read each char
		str += infile.read(1,50000000);
	}

	//puts("read " + str.length.to_str + " chars...");

	return str;
}

int main () {

	// file stream type
	io::file myfile;

		// open for read
		if(!myfile.open("sometextfile", "r"))
			io::perror("error reopening file");
		else {

			// writes to output file
			io::puts(read_file(myfile));

			// close file
			myfile.close;
		}

    return 0;
}