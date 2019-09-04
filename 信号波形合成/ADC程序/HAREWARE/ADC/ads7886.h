#ifndef _ADS7886_H
#define _ADS7886_H
#include<common.h>

#define SCLK_H GPIO_SetBits (GPIOB, GPIO_Pin_8);
#define SCLK_L GPIO_ResetBits (GPIOB, GPIO_Pin_8);
#define CS_H		GPIO_SetBits (GPIOA, GPIO_Pin_1);
#define CS_L		GPIO_ResetBits (GPIOA, GPIO_Pin_1);
#define SDO_read GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)

 extern u16 ADS7886_sumpling(void);
 extern u16 ads7886_select(void);
	extern void gpio_Init(void);
#endif
