#include "MatrixKey.h"

unsigned char KeyDown(){
	char a = 0;
	unsigned char KeyVal = 0;
	
	P1 = 0x0f;// 0000 1111  行置低 列置高
	
	if(P1 != 0x0f){
		Delay(20);
		if(P1 != 0x0f){
			//测试列
			switch(P1){
				case(0x07): KeyVal = 1;break; // 0000  0111         8 4 2 1
				case(0x0B): KeyVal = 2;break; // 0000  1011         8 4 2 1
				case(0x0D): KeyVal = 3;break; // 0000  1101         8 4 2 1
				case(0x0E): KeyVal = 4;break; // 0000  1110         8 4 2 1
			}
			//测试行
			P1 = 0xF0;
			switch(P1){
				case(0x70): KeyVal = KeyVal;break; 		// 0111  0000         8 4 2 1
				case(0xB0): KeyVal = KeyVal + 4;break; 	// 1011  0000         8 4 2 1
				case(0xD0): KeyVal = KeyVal + 8;break; 	// 1101  0000         8 4 2 1
				case(0xE0): KeyVal = KeyVal + 12;break; // 1101  0000         8 4 2 1
			}
			while((a < 50) && (P1 != 0xF0))
			{
				Delay(20);
				a++;
			}
		}
		return KeyVal;
	}else{
		return 0;
	}
}


