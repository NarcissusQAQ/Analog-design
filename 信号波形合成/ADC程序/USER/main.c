#include "common.h"

int main(void)
{
	int j;
	int a,b; //a为小数点左       b位小数点右
	GPIO_InitTypeDef GPIO_InitStructure;         //定义GPIO—init中 用到的结构体	
	u16 adc,adc_avg;
	delay_init();
	Lcd_Init();
	LCD_Display_Dir(1);    //横屏
	LCD_Clear(WHITE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);	  //初始化IO口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	  //初始化IO口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	  //初始化IO口时钟
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;          //下拉输入  SDO
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;	
		GPIO_Init(GPIOD,&GPIO_InitStructure); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //推挽输出  sclk
			GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure); 
		GPIO_ResetBits (GPIOB, GPIO_Pin_8); 
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;          //推挽输出  cs
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure); 
		GPIO_SetBits (GPIOA, GPIO_Pin_1);
	 POINT_COLOR=RED;//设置字体为红色	
				
				putxGB(158,15,"峰值检测",24);
				putxGB(31,50,"10K正弦波:    .    V",24);
				putxGB(31,95,"30K正弦波:    .    V",24);
			 putxGB(31,140,"50K正弦波:    .    V",24);
				putxGB(31,185," 方   波 :    .    V",24);
				LCD_DrawLine(5,5,315,5);
				LCD_DrawLine(5,5,5,235);
				LCD_DrawLine(5,235,315,235);
				LCD_DrawLine(315,5,315,235);
			POINT_COLOR=BLACK;//设置字体为蓝色		
				putxGB(7,215,"夏",16);			
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

				LCD_ShowxNum(10,10,adc,4,16,0);//显示adc				
				
				LCD_ShowxNum(180,186,a,1,24,0);//显示电压值				方波的a
			  LCD_ShowxNum(214,186,b,3,24,0);//显示电压值				方波的b
		
				LCD_ShowxNum(180,50,0,1,24,0);//显示电压值				10k正弦波的a
			  LCD_ShowxNum(214,50,000,3,24,0);//显示电压值				100k正弦波的b
				
				LCD_ShowxNum(180,95,0,1,24,0);//显示电压值				30k正弦波的a
			  LCD_ShowxNum(214,95,000,3,24,0);//显示电压值				30k正弦波的b
				
				LCD_ShowxNum(180,140,0,1,24,0);//显示电压值				50k正弦波的a
			  LCD_ShowxNum(214,140,000,3,24,0);//显示电压值				50k正弦波的b
				
				delay_ms(2);

			
	}
return 0;
}

//		
//		POINT_COLOR=BLUE;//设置字体为蓝色			
//		LCD_Fill(x2+2,31,x2+10,233,WHITE);
//		POINT_COLOR=BLUE;	
//  	LCD_ShowxNum(256,7,adc1,1,16,0);//显示电压值
//		LCD_ShowxNum(256,7,adc1,1,16,0);//显示电压值

