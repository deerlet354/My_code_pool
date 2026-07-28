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
	
	unsigned char ledNum = 0x00;
	
	P2 = ~0x01;//好吧，上电初始化是最稳的
	
	while(1){
		if(P3_0 == 0){
			Delay(20);
			while(P3_0 == 0);
			Delay(20);
			
			if(ledNum == 7) ledNum = 0;
			else ledNum++;
			
			P2 = ~(0x01 << ledNum);
			
			
		}else if(P3_1 == 0){
			Delay(20);
			while(P3_1 == 0);
			Delay(20);
			
			if(ledNum == 0) ledNum = 7;
			else ledNum--;
			
			P2 = ~(0x01 << ledNum);
			
		}
	}
}
