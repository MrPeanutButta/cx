
/* standard type member functions example
 * 
 * var.to_str() - returns the string copy of the valuw of var
 */

#include io

int main () {

	int *a = {
		0,1,2,3,4,5,6,7,8,9
	};

	int i = 0;
	do{
		io::puts(a[i].to_str);
	}while(i++ < a.length);

    return 0;
}