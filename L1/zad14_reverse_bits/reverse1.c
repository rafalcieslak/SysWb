unsigned int reverse(unsigned int x){
	unsigned int t = 0;
	unsigned int mask = 1;
	unsigned int reversemask = 1<<31;
	do{
		t |= ((x&mask)!=0)*reversemask;
		mask <<= 1;
		reversemask >>= 1;
	}while(reversemask > 0);
	return t;
}
