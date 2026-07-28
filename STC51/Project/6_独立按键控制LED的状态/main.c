#include <REGX52.H>

void Delay(unsigned int xms)
{
	unsigned char data i, j;

	while(xms){
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
		xms --;
	}
}


void main(){
	
	P2_0 = 1;
	while(1){
		if(P3_0 == 0){
			Delay(20);
			while(P3_0 == 0);
			Delay(20);
			
			P2_0 = !P2_0;
		}
	}
}
