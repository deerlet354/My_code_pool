#include <REGX52.H>
#include <string.h>

unsigned char str[8] = {0XFE,0XFD,0XFB,0XF7,0XEF,0XDF,0XBF,0X7F};

void Delay1ms(unsigned int xms)	//@12.000MHz
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
	
	P2 = 0XFE;
	while(1){
		int i = 0;
		for(;i < strlen(str);i ++){
			P2 = str[i];
			Delay1ms(100);
		}
		for(i = strlen(str) - 1;i >= 0;i --){
			P2 = str[i];
			Delay1ms(50);
		}
	}
}
