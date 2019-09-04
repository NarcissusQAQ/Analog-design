#include "common.h"

int main(void)
{
	int j;
	int a,b; //aΪС������       bλС������
	GPIO_InitTypeDef GPIO_InitStructure;         //����GPIO��init�� �õ��Ľṹ��	
	u16 adc,adc_avg;
	delay_init();
	Lcd_Init();
	LCD_Display_Dir(1);    //����
	LCD_Clear(WHITE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);	  //��ʼ��IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	  //��ʼ��IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	  //��ʼ��IO��ʱ��
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;          //����������  SDO
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;	
		GPIO_Init(GPIOD,&GPIO_InitStructure); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //�������  sclk
			GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure); 
		GPIO_ResetBits (GPIOB, GPIO_Pin_8); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //�������  cs
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure); 
		GPIO_SetBits (GPIOA, GPIO_Pin_1);
	 POINT_COLOR=RED;//��������Ϊ��ɫ	
				
				putxGB(158,15,"��ֵ���",24);
				putxGB(31,50,"10K���Ҳ�:    .    V",24);
				putxGB(31,95,"30K���Ҳ�:    .    V",24);
			 putxGB(31,140,"50K���Ҳ�:    .    V",24);
				putxGB(31,185," ��   �� :    .    V",24);
				LCD_DrawLine(5,5,315,5);
				LCD_DrawLine(5,5,5,235);
				LCD_DrawLine(5,235,315,235);
				LCD_DrawLine(315,5,315,235);
			POINT_COLOR=BLACK;//��������Ϊ��ɫ		
				putxGB(7,215,"��",16);			
	while(1)
	{
					adc_avg=0;
				for(j=0;j<10;j++)
		{
					adc=ads7886_select();
					adc_avg+=adc;
		}
					adc_avg=adc_avg/10;
		
			a= adc_avg*(11)/4096;			//			a= adc_avg*(3.3)/4096;
			b= (adc_avg*4200/4096);
				//j++;

				LCD_ShowxNum(10,10,adc,4,16,0);//��ʾadc				
				
				LCD_ShowxNum(180,186,a,1,24,0);//��ʾ��ѹֵ				������a
			  LCD_ShowxNum(214,186,b,3,24,0);//��ʾ��ѹֵ				������b
		
				LCD_ShowxNum(180,50,0,1,24,0);//��ʾ��ѹֵ				10k���Ҳ���a
			  LCD_ShowxNum(214,50,000,3,24,0);//��ʾ��ѹֵ				100k���Ҳ���b
				
				LCD_ShowxNum(180,95,0,1,24,0);//��ʾ��ѹֵ				30k���Ҳ���a
			  LCD_ShowxNum(214,95,000,3,24,0);//��ʾ��ѹֵ				30k���Ҳ���b
				
				LCD_ShowxNum(180,140,0,1,24,0);//��ʾ��ѹֵ				50k���Ҳ���a
			  LCD_ShowxNum(214,140,000,3,24,0);//��ʾ��ѹֵ				50k���Ҳ���b
				
				delay_ms(2);

			
	}
return 0;
}

//		
//		POINT_COLOR=BLUE;//��������Ϊ��ɫ			
//		LCD_Fill(x2+2,31,x2+10,233,WHITE);
//		POINT_COLOR=BLUE;	
//  	LCD_ShowxNum(256,7,adc1,1,16,0);//��ʾ��ѹֵ
//		LCD_ShowxNum(256,7,adc1,1,16,0);//��ʾ��ѹֵ

