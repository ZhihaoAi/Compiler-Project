/******************************************
	Default test file for MP language:
	Bubble sort
*******************************************/
a[0] = 10;
a[1] = 1;
a[2] = 6;
a[3] = -4;
a[4] = 9;
a[5] = 15;
a[6] = 77;
a[7] = 46;
a[8] = -31;
a[9] = 0;

for(x=0; x<10; x=x+1;)
	for(y=0; y<9; y=y+1;)
		if(a[y] > a[y+1]){
			t = a[y];
			a[y] = a[y+1];
			a[y+1] = t;
		}

// Print out the sorted array:
// -31
// -4
// 0
// 1
// 6
// 9
// 10
// 15
// 46
// 77
for(x=0; x<10; x=x+1;) print a[x];

// Test continue/break:
// Print the even numbers that is not greater than 10 in a[].
x=0;
while(x<10) {
	if (!(a[x] % 2 == 1 || a[x] % 2 == -1))
		if (a[x] > 10)
			break;
		else
			print a[x];
	x=x+1;
}
// Test read.
// read x;
// print x;
