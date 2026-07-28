#include <REGX52.H>

void Delay(unsigned int xms)
{
	unsigned char data i, j;

	while(xms--){
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}


void main(){
	
	unsigned char ledNum = 0x00;
	
	while(1){
		if(P3_0 == 0){
			Delay(20);
			while(P3_0 == 0);
			Delay(20);
			
			ledNum++;
			P2 = ~ledNum;
		}
	}
}
