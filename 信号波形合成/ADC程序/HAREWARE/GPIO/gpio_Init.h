#ifndef __GPIO__H
#define __GPIO__H         

#define SCLK_H GPIO_SetBits (GPIOB, GPIO_Pin_8);
#define SCLK_L GPIO_ResetBits (GPIOB, GPIO_Pin_8);
#define CS_H		GPIO_SetBits (GPIOA, GPIO_Pin_1);
#define CS_L		GPIO_ResetBits (GPIOA, GPIO_Pin_1);
#define SDO_read GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)
void gpio_Init(void);




#endif



