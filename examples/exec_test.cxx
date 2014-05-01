#include io
#include char

/** @param void
 */
int main () {

    char *c = "some long ass string to_str splat";
	char *a = c.to_str;

	for (int i = 0; i < a.length; i++){
		if (a[i].is_xdigit) {
			io::puts(a[i].to_upper.to_str);
		}
	}

    //io::puts(a.to_str.length.to_str);
 
    return 0;
}