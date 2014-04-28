/* Array initialization examples
 */
#include math

int main () {

    // array initialization list
    int *b = {
              4096 * 1,
              4096 * 2,
              4096 * 3,
              4096 * 4,
              4096 * 5
    };

	 // sqr becomes {new array} + b + b + {new array}
    int *sqr = {0,1,2,3,4} + b + b + {23,2,43234,234};

	// array a will contain the contents of all arrays in the expression
	int *a = b + b;

    do {
        // allocate new int, a full array, AND the i index of sqr at the end
        sqr += math::isqrt(b[i]) + a + sqr[i];
    } while (i++ < 5);

    return 0;
}