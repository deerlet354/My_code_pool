#ifndef __MOTOR_H
#define __MOTOR_H


#include "pid.h"
#include "main.h"
#include "AS5600_PWM.h"
#include "stm32g0xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


typedef struct {
    struct {
		uint16_t MOTOR_MIDPOINT;
        uint16_t Corrective_Angle;   // 校正后角度（经过零位校准和滤波处理后的角度）
        int16_t Target_Angle;       // 目标角度（PID控制的目标位置，单位与Corrective_Angle一致）
        int16_t Target_Speed;       // 电机转速（单位：RPM 或 度/秒，根据具体配置决定）
		int8_t encoder_dir;
		int8_t motor_dir;
    } Wings_motor[5];   // 翅翼电机数组：[0]左前翅, [1]右前翅, [2]左后翅, [3]右后翅, [4]预留或特殊用途
    
} WINGS_DATA;

enum
{
	Default, 		
	Encoder_cali, 	
	Motor_cali,
	Flash_write,
	Cali_ok,

	NONE,
};


extern uint8_t state;
extern WINGS_DATA Wings_Data;
extern pid_type_def motor_1_pid,motor_3_pid;
#define KP 	20.0f
#define KD  00.0f

#define MOTOR_1_SPEED_PID_KP KP
#define MOTOR_1_SPEED_PID_KI 0.0f
#define MOTOR_1_SPEED_PID_KD KD
#define MOTOR_3_SPEED_PID_KP KP
#define MOTOR_3_SPEED_PID_KI 0.0f
#define MOTOR_3_SPEED_PID_KD KD

// 1	2
// 
// 4	3






/*-------------Motor_PWM_M1--------------*/
#define PWM_M1_1 	  TIM2->CCR1	 //PWM_M1
#define PWM_M1_2 	  TIM2->CCR2	 //PWM_M1
/*------------------------------------*/


/*-------------Motor_PWM_M3--------------*/
#define PWM_M3_1 	  TIM3->CCR1	 //PWM_M3
#define PWM_M3_2 	  TIM3->CCR2	 //PWM_M3

/*------------------------------------*/

extern void Motor_PID_Control(void);
extern void Chassis_PID_Init(void);
extern void Set_Pwm(int16_t motor1_out, int16_t motor2_out, int16_t motor3_out, int16_t motor4_out);
extern uint16_t myabs(int16_t a);
extern void Motor_ECD_Control(void);
int16_t Encoder_unwrap(int16_t value);
int8_t Get_encoder_dir(int16_t value);
uint16_t Get_correct_ad(int8_t encoder_dir, uint16_t ad_value);
int8_t Compare(int16_t value, int16_t max, int16_t min);
#endif
