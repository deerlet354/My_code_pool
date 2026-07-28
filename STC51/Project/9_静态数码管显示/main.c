#include <REGX52.H>

//段码表 					  0    1    2    3    4    5    6    7    8    9
unsigned char ledArr[10] = {0X3F,0X06,0X5B,0X4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F};
//P2.2 P2.3 P2.4 即就是P2的D2-D4  
//则只需变这三位，其余位不变 
//0:000 000 00  1:000 001 00  2:000 010 00  3:000 011 00 4:000 100 00 ...
//unsigned char ledWei[8] = {0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C};

void Nixie(unsigned char Location,unsigned char Number){
	switch(Location){
		case 1:P2_4 = 0;P2_3 = 0;P2_2 = 0;break;
		case 2:P2_4 = 0;P2_3 = 0;P2_2 = 1;break;
		case 3:P2_4 = 0;P2_3 = 1;P2_2 = 0;break;
		case 4:P2_4 = 0;P2_3 = 1;P2_2 = 1;break;
		case 5:P2_4 = 1;P2_3 = 0;P2_2 = 0;break;
		case 6:P2_4 = 1;P2_3 = 0;P2_2 = 1;break;
		case 7:P2_4 = 1;P2_3 = 1;P2_2 = 0;break;
		case 8:P2_4 = 1;P2_3 = 1;P2_2 = 1;break;
	}
	P0 = ledArr[Number];
}

//void Nixie(unsigned char Location,unsigned char Number){
//	P2 = (P2 & 0xE3) | ledWei[Location - 1];
//	P0 = ledArr[Number];
//}

void main(){
	
	Nixie(4,6);
	
	while(1){
	}
}