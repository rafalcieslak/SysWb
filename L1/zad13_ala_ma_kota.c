#include <stdio.h>
#include <string.h>

void funkcja_z_zad13(char* s, size_t n){
	char* p = s+n-1; // last char
	char* r = p;
	while(r >= s){
		while(r >= s && *r == ' ') r--;
		if(r >=s) *p = *r;
		p--; r--;
	}
	while(p >= s) *p-- = ' ';
}

int main(){
	char s[1000];
	fgets(s, 1000, stdin);
	funkcja_z_zad13(s, strlen(s)-1); //strip newline
	printf("%s",s);
}
