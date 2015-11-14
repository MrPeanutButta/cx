
int t = 600;
int a = 1;
int b = a;

if((a * b) > t){
	return 1;
} else if((a * t) > 5000){
	return 2;
} else if(a == b) return 3;

return 0;