#include <Delay_Mode1.h>

volatile unsigned int ms_counter = 0;
volatile bit timer_500ms_flag = 0;//500ms到达标志

void Timer0_Init_1ms_IT(void){
	TMOD &= 0xF0;//清空低四位，保留高四位配置
	TMOD |= 0x01;//方式1，16位定时器
	TH0 = 0XFC;
	TL0 = 0X18;
	TF0 = 0;
	EA = 1; //开启总中断允许
	ET0 = 1;//开始定时器0的中断允许
	TR0 = 1;//允许定时器0开始计数
}

//T0中断服务函数
void Timer0_ISR(void) interrupt 1{
	//重新赋初值，为下一次1ms中断做准备
	TH0 = 0XFC;
	TL0 = 0X18;
	//每次中断，毫秒计数器加1
	ms_counter ++;
	
	if(ms_counter >= 500){
		timer_500ms_flag = 1;
		ms_counter = 0;
	}
}