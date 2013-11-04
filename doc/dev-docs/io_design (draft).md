Cx standard I/O design document

[07-27-2013] Ideas for basic I/O design
Initial design for I/O in Cx includes the use of 3 symbol table nodes
pointed to by p_stdin, p_stdout, and p_stderr. These nodes serve the purpose of 
basic I/O.

Largely focusing on output until final draft, at which point input will be drafted.

configuration:
type name = 'file'
cx_defn_code = dc_type;
cx_form_code = fc_stream;

read and write operations would be baked into the interpretor and stdin, stderr,
and stdout would be predefined types of 'file'

Open files would be tracked by adding an additional linked list of I/O objects to


Cx implementation example:

/* partial pseudo code */

int main(){
    file file_object;
                                       //ios::out/in/bin like flags
    file_object = fopen("example.txt", some_open_flag)

    fwrite(file_object, "write something to the file", length);

    char buffer[size];
    fread(file_object, buffer, length);
}

Pros:
    This design handles file and stdout/in.
    Mid-level of difficulty.
    Will enable the ability to use C like functions to write Cx fstream(s).
    Most likely the best solution.
Cons:
    Interpretor will need built in read/write functions.
    

--end proposal #4

