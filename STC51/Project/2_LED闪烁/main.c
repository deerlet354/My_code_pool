#include <REGX52.H>
#include <INTRINS.H>

void Delay500ms() //12MHZ   T = 12 / fosc = 12 / 12 000 000 = 1us
{ 
	unsigned char i,j,k;
	
	_nop_(); //空指令，占一个机器周期，1us
	i = 4;
	j = 205;
	k = 187;
	do{
		do{
			while (--k);
		}while(--j);
	}while(--i);
}

void main(){

	while(1){
		P2 = 0XFE;
		Delay500ms();
		P2 = 0XFF;
		Delay500ms();
	}
}
