#include io

int main () {

	io::file myfile;
	char *buffer = "test - I will write this test to a text file then read it back";
	char *filename = "textfile.txt";

	io::puts(buffer + ": I will now attempt to r/w to " + filename);
	
	if(!myfile.open(filename, "w"))
		io::perror("error opening file: " + filename);
	else {
			
		myfile.puts(buffer);
		myfile.close;
	}

	if(!myfile.reopen(filename, "r"))
		io::perror("error opening file: " + filename);
	else {
		while(!myfile.eof){
			io::puts(myfile.getc.to_str);
		}
		myfile.close;
	}

	if(!rename(filename, "new_file_name")){
		io::perror("unable to rename " + filename);
	} else {
		io::puts("renamed " + filename + " to new_file_name\n");
	}

	if(io::remove("new_file_name")){
		io::puts("removed file");
	} else {
		io::perror("remove");
	}

    return 0;
}