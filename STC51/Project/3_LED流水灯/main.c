#include <REGX52.H>
#include <INTRINS.H>
#include <string.h>

unsigned char str[8] = {0XFE,0XFD,0XFB,0XF7,0XEF,0XDF,0XBF,0X7F};

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


void Mode1(){
	while(1){
		P2 = 0XFE; // 1111 1110
		Delay500ms();
		P2 = 0XFD; // 1111 1101
		Delay500ms();
		P2 = 0XFB; // 1111 1011
		Delay500ms(); 
		P2 = 0XF7; // 1111 0111
		Delay500ms();
		P2 = 0XEF; // 1110 1111
		Delay500ms();
		P2 = 0XDF; // 1101 1111
		Delay500ms();
		P2 = 0XBF; // 1011 1111
		Delay500ms();
		P2 = 0X7F; // 0111 1111
		Delay500ms();
	}
}

void Mode2(){
	P2 = 0XFE;
	while(1){
		Delay500ms();
		P2 = (P2 << 1) | 0x01; //左移后末尾补1
	
		if(P2 == 0x7F){
			P2 = 0xFE;
		}
		else{
			P2 == 0x7F;
		}
	}
}

void Mode3(){
	P2 = 0XFE;
	while(1){
		Delay500ms();
		P2 = (P2 << 1) | 0x01; //左移后末尾补1
	
		if(P2 == 0x7F){
			while(1){
				Delay500ms();
				P2 = (P2 >> 1) | 0x80; //右移后首位补1
				if(P2 == 0xFE){
					break;
				}
				else{
					P2 == 0xFE;
				}
			}
		}
		else{
			P2 == 0x7F;
		}
	}
}

void Mode4(){
	while(1){
		int i = 0;
		for(;i < strlen(str);i ++){
			P2 = str[i];
			Delay500ms();
		}
		for(i = strlen(str) - 1;i >= 0;i --){
			P2 = str[i];
			Delay500ms();
		}
	}
	
}


void main(){
	//Mode1();
	//Mode2();
	//Mode3();
	Mode4();
}
