
/* gets() and string allocation example
 */
#include io

int main () {

    do{
        io::puts("Please enter your name: ");
        // read string until '\n'
        char *in = io::gets();
    }while(!in);
    
    char *t1 = "beep";
    char *t2 = "-boop";
    char *greet = "Hello " + in + ", you a bad " + t1 + t2 + " !";

    io::puts(greet);

    return 0;
}