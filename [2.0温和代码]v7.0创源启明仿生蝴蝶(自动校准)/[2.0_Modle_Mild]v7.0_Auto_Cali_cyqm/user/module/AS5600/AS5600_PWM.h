#ifndef __AS5600_PWM_H__
#define __AS5600_PWM_H__
#include "main.h"
#include "adc.h"

#define MAX_VALUE 4095  // 输入范围 0~4095，所以最大值为 4096

#define PROCESS_VALUE(raw, zero) \
    (((raw) + 4096u - (((zero) + 3072u) & 0x0FFFu)) & 0x0FFFu)

extern uint16_t AD_Value[4];

void StarAndGetResult(void);

#endif
