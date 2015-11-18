
int a = 3;
int b = 6;
int c = 0;
int d = 5;
int i = 0;

/*while(a < 1000000) {
	a++;
}

*/
while(i < 100000)	{
	c += a++ * b++ + d++;
	i++;
}


//while(letter < 'z') letter++;

return c;