#include <stdio.h>
#include <stdlib.h>

unsigned int reverse(unsigned int x);

int main(int argc, char** argv){
	unsigned int sum = 0;
	if(argc < 2){
		printf("Pass n as commandline argument.\n");
		return 0;
	}
	unsigned int n = atoi(argv[1]);
	for(unsigned int i = 0; i < n; i++){
		sum ^= reverse(i);
	//	printf("0x%x\n",reverse(i));
	}
	printf("%x\n",sum); // prevents from optimizing out anything;
}
