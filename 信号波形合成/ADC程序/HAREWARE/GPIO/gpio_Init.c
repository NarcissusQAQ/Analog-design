#include <gpio_Init.h>
#include <stm32f10x.h>

void gpio_Init(void)
{
			GPIO_InitTypeDef GPIO_InitStructure;         //定义GPIO—init中 用到的结构体	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);	  //初始化IO口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	  //初始化IO口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	  //初始化IO口时钟
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;          //下拉输入
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;	
		GPIO_Init(GPIOD,&GPIO_InitStructure); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //推挽输出
			GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure); 
		GPIO_ResetBits (GPIOB, GPIO_Pin_8); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //推挽输出
			GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure); 
		GPIO_SetBits (GPIOA, GPIO_Pin_0); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //推挽输出
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure); 
		GPIO_SetBits (GPIOA, GPIO_Pin_1);
}

