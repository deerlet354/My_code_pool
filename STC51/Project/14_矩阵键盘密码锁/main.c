#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "MatrixKey.h"

unsigned char KeyNum = 0;
unsigned int password = 0,Count = 0;

void main(){
	
	LCD_Init();
	LCD_ShowString(1,1,"password:");
	
	while(1){
		
		KeyNum = KeyDown();
		if(KeyNum){
			if(KeyNum <= 10){ //S1-S12按键为密码锁功能范围，S1-S10是密码其余4个按键没用到
				if(Count < 4){ 				 //限制次数
					password *= 10;			 //拼接密码
					password += KeyNum % 10; //获取一位密码
					Count ++;
				}
				LCD_ShowNum(2,1,password,4);
			}
			if(KeyNum == 11){//确认键
				if(password == 2345){
					LCD_ShowString(1,14," OK");
					password = 0;
					Count = 0;
					LCD_ShowNum(2,1,password,4);
				}
				else{
					LCD_ShowString(1,14,"ERR");
					password = 0;
					Count = 0;
					LCD_ShowNum(2,1,password,4);
				}
			}
			if(KeyNum == 12){//取消键
				password = 0;
				Count = 0;
				LCD_ShowString(1,14,"   ");
				LCD_ShowNum(2,1,password,4);
			}
		}
			
	}
}
