#include io

namespace std {

	namespace std {

		namespace std {

			// 3 namespaces deep
			int f(int i){
				return (i * 5);
			}
		}

		// 2 deep
		int f(int i){	
			return (i * 6);
		}
	}
	
	// 1 deep
	int f(int i){
		return (i * 7);
	}
}

int main () {
	
	int a = std::L_tmpnam;

	std::puts(std::f(a).to_str);
	std::puts(std::std::f(a).to_str);
	std::puts(std::std::std::f(a).to_str);

	std::puts(std::f(std::std::f(std::std::std::f(a))).to_str);
    return 0;
}