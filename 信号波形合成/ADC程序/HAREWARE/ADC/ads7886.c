/*ads7886Ϊ����AD�ź���һ��Ҫ�ֿ��õ����ߣ����������ֵƫ��50MV*/
#include <ads7886.h>
#include <common.h>
#include <stm32f10x.h>



u16 ads7886_select(void)
{
  u16 dat=0;
  char i;
   dat=0;
   
   CS_H; //cs = 1;
  
   CS_L;//cs = 0;
   
 for(i=0;i<3;i++)
 { 
  SCLK_H;
			__nop();
	 		__nop();
  SCLK_L;
	 		__nop();
	 		__nop();

 }
  for(i=0;i<12;i++)
  {
   	  
      SCLK_H;
				__nop();
				__nop();	
  
    SCLK_L;
					__nop();
					 dat<<=1;	 
					if(SDO_read ==1)   
						dat++;

	 
    }
	 	
   	 CS_H;
		 	__nop();
    return dat;
  
}

void gpio_Init(void)
{
			GPIO_InitTypeDef GPIO_InitStructure;         //����GPIO��init�� �õ��Ľṹ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);	  //��ʼ��IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	  //��ʼ��IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	  //��ʼ��IO��ʱ��
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;          //����������
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;	
		GPIO_Init(GPIOD,&GPIO_InitStructure); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //�������
			GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure); 
		GPIO_ResetBits (GPIOB, GPIO_Pin_8); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //�������
			GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure); 
		GPIO_SetBits (GPIOA, GPIO_Pin_0); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //�������
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure); 
		GPIO_SetBits (GPIOA, GPIO_Pin_1);
}



