/*
 this is a block comment.
*/

/*int assembly_test_0(int i){
	asm{
		iconst 0b11111111
		iconst 0xff
		iadd
		istore i
		iload i
		istore assembly_test_0
		ret
	}
}*/

byte binary_test_0(){
	return 0b11111111;
}

byte hex_test_0(){
	return 0xff;
}

byte octal_test_0(){
	return 0377;
}

/*int operator_test_0(){
	bool return_passing = false;
	
	int i = 0;
	char c = 'a';
	real r = 2.0;
	byte z0 = 0b11111111;	// 255
	byte z1 = 0xff;			// 255
	
	return z0;
}*/

int benchmark_test_0(){
	int s = 0;
	int i = 0;
	int j = 0;
	
	for (; i < 10000;)
		for (j = 0; j < 10000;) 
			s += i++ * j++; 
	
	return s;
}

int recursive_test_0(int count){
	if(count == 50) return count;
	// count++ will continue to pass initial value of
	// count and cause a stack overflow.
	return recursive_test_0(++count);
}

// while loop tests
int while_test_0(int count){
	int a = 3;
	int b = 6;
	int c = 0;
	int d = 5;
	int i = 0;

	while(i < count)	{
		c += a++ * b++ + d++;
		i++;
	}
	
	return c;
}

// if statement tests
int if_test_0(int count){
	int i = 0;
	int return_value = 50;
	
	while(i < count){
		if(i >= return_value){
			i = 0;
			while(i < return_value) i++;
			
			return i;
		} else {
			i++;
		}
		
		if((i % 2) == 0){
			// even number
			int dummy = 0;
		} else {
			// odd number
			int dummy = 5;
		}
	}
	
	return 0;
}

int do_test_0(int count){
	int i = 0;
	int a = 0;
	
	do{
		a = i;
	} while(i++ < count);
	
	return a;
}

int break_test_0(int count){
	int i = 0;
	
	do{
		i++;
		if(i == count) break;
	}while(i < count + 5);
	
	return i;
}

int break_test_1(int count){
	int i = 0;
	
	while(true){
		i++;
		if(i == count) break;
	}
	
	return i;
}

int for_test_0(int count){
	int a = 0;

	for(int i = 0; i < count; i++){
		a = i;
	}
	
	for(int i = 0; i < count; i++){
		a = i;
	}
	
	int i = 0;
	for(a = 0; a < count;){
		i = a;
		a++;
	}
	
	for(;a < count + 100;){
		a++;
	}
	
	for(;;){
		if(a++ == 1000) break;
	}
	
	return count;
}

bool pass_check(bool expression){
	return expression; 
}

bool for_test_0_pass = false;
bool while_pass = false;
bool if_pass = false;
bool do_pass = false;
bool break_0_pass = false;
bool break_1_pass = false;
bool recursive_test_0_pass = false;
bool binary_test_0_pass = false;
bool hex_test_0_pass = false;
bool octal_test_0_pass = false;

if(pass_check(recursive_test_0(0) == 50)) recursive_test_0_pass = true;
if(pass_check(pass_check(pass_check(pass_check((while_test_0(100) == 380150)))))) while_pass = true;
if(pass_check((if_test_0(200) == 50)) == true) if_pass = true;
if(pass_check((do_test_0(100) == 100)) == true) do_pass = true;
if(pass_check((while_test_0(100) == 380150)) == true) while_pass = true;
if(pass_check((if_test_0(200) == 50)) == true) if_pass = true;
if(pass_check((do_test_0(100) == 100)) == true) do_pass = true;
if(pass_check((while_test_0(100) == 380150)) == true) while_pass = true;
if(pass_check((if_test_0(200) == 50)) == true) if_pass = true;
if(pass_check((do_test_0(100) == 100)) == true) do_pass = true;
if(pass_check((while_test_0(100) == 380150)) == true) while_pass = true;
if(pass_check((if_test_0(200) == 50)) == true) if_pass = true;
if(pass_check((do_test_0(100) == 100)) == true) do_pass = true;
if(pass_check((break_test_0(20) == 20))) break_0_pass = true;
if(pass_check((break_test_1(100) == 100))) break_1_pass = true;
if(pass_check((for_test_0(100) == 100))) for_test_0_pass = true;
if(pass_check((binary_test_0() == 255))) binary_test_0_pass = true;
if(pass_check((hex_test_0() == 255))) hex_test_0_pass = true;
if(pass_check((octal_test_0() == 255))) octal_test_0_pass = true;

return benchmark_test_0();