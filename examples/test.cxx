
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
		
		if((i / 2) == 0){
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
		//i++;
		a = i;
	} while(i++ < count);
	
	return a;
}

int break_test_0(int count){
	int i = 0;
	
	do{
		i++;
		if(i == count) break;
	}while(true);
	
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

bool while_pass = false;
bool if_pass = false;
bool do_pass = false;
bool break_0_pass = false;
bool break_1_pass = false;

if(while_test_0(100) == 380150) while_pass = true;
if(if_test_0(200) == 50) if_pass = true;
if(do_test_0(100) == 100) do_pass = true;
if(break_test_0(20) == 20) break_0_pass = true;
if(break_test_1(100) == 100) break_1_pass = true;

return break_1_pass;