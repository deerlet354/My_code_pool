#include <Delay_Mode2.h>

volatile unsigned int interrupt_cnt = 0;//中断次数计数器
volatile bit timer_1ms_flag = 0;//1ms到达标志

//延时1ms
void Timer0_Init_1ms_IT(void){
	TMOD &= 0xF0;//清空低四位，保留高四位配置
	TMOD |= 0x02;//方式1，16位定时器
	TH0 = 0x06;
	TL0 = 0x06;
	TF0 = 0;
	EA = 1; //开启总中断允许
	ET0 = 1;//开始定时器0的中断允许
	TR0 = 1;//允许定时器0开始计数
}

//T0中断服务函数
void Timer0_ISR(void) interrupt 1{
	//每次中断，毫秒计数器加1
	interrupt_cnt ++; //累加中断次数（每250us加1）
	
	if(interrupt_cnt >= 4){//累加4次 = 250us * 4 = 1000us = 1ms
		interrupt_cnt = 0;
		timer_1ms_flag = 1;
	}
}