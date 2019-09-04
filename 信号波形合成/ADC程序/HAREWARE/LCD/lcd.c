
/*SYSTEM*/
#include "delay.h"
#include "sys.h"
#include "string.h"
/*HARDWARE*/
#include "lcd.h"
#include "font.h"
#include "GB3232.h"
#include "GB1616.h"
#include "GB2424.h"
/* http://ttmcu.taobao.com 
代码测试环境:单片机STC12LE5A60S2,晶振33M  单片机工作电压3.3V
程序默认IO连接方式：
控制线：RS-P3^5;    WR-P3^6;   RD-P3^7;   CS-P1^0;   REST-P1^2;
数据线: DB0-DB7依次连接P0^0-P0^7;  DB8-DB15依次连接P2^0-P2^7;
触摸功能连接方式：(不使用触摸可不连接)
T_CLK-P1^7;  T_CS-P1^4;  T_DIN-P3^0;  T_OUT-P3^1;  T_IRQ-P3^4;
*/
u16 BACK_COLOR=WHITE, POINT_COLOR=BLACK;   //背景色，画笔色
u8 DFT_SCAN_DIR = L2R_U2D;


//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

void LCD_Writ_Bus(u16 data)   //并行数据写入函数
{
	GPIO_Write(GPIOD,data);
	LCD_WR=0;
	LCD_WR=1; 
}
void LCD_WR_DATA8(u16 data) //发送数据-8位参数
{
    LCD_RS=1;
		LCD_Writ_Bus(data);
}  
void LCD_WR_DATA(int da)
{
    LCD_RS=1;
		LCD_Writ_Bus(da);
}	  
void LCD_WR_REG(int da)	 
{	
    LCD_RS=0;
		LCD_Writ_Bus(da);
}
 void LCD_WR_REG_DATA(int reg,int da)
{
    LCD_WR_REG(reg);
		LCD_WR_DATA(da);
}
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{ 
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(x1>>8);
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2>>8);
	LCD_WR_DATA(x2);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(y1>>8);
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2>>8);
	LCD_WR_DATA(y2);	
	LCD_WR_REG(0x2c);					 						 
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
// 	if(lcddev.dir==1)
//	{
//		u16 t;
//		t=Xpos;
//		Xpos=lcddev.height-1-Ypos;//横屏其实就是调转x,y坐标
//		Ypos=t;
//	}
	Address_set(Xpos,Ypos,Xpos,Ypos);
} 

//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510等IC已经实际测试	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	switch(dir)
	{
		case L2R_U2D://从左到右,从上到下
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://从下到上,从右到左
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36;
	regval|=0X08;	//65K 
	LCD_WR_REG_DATA(dirreg,regval);
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF); 
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		lcddev.wramcmd=0x2c;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B; 
		DFT_SCAN_DIR = L2R_U2D;
	}
	else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		lcddev.wramcmd=0x2c;
		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;
		DFT_SCAN_DIR = L2R_D2U;
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}

void Lcd_Init(void)
{
	//GPIOD初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
   	LCD_CS =1;
//	if(LCD_CS==0)
//	{
//	   LCD_WR_REG_DATA(0,0);
//	   LCD_ShowString(0,0," ");
//	   LCD_ShowNum(0,0,0,0);
//	   LCD_Show2Num(0,0,0,0);
//	   LCD_DrawPoint_big(0,0);
//	   LCD_DrawRectangle(0,0,0,0);
//	   Draw_Circle(0,0,0);
// 	 }	   
//	LCD_RD=1;
	LCD_WR=1;
	LCD_REST=0;
	delay_ms(20);	//此处保证要有5ms以上才能保证复位稳定,推荐20ms
	LCD_REST=1;	
	delay_ms(20);	//此处保证要有5ms以上才能保证复位稳定,推荐20ms		
	LCD_CS =0;  //打开片选使能

	//************* Start Initial Sequence **********//
	LCD_WR_REG(0xcf); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xc1);
	LCD_WR_DATA(0x30);

	LCD_WR_REG(0xed); 
	LCD_WR_DATA(0x64);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x81);

	LCD_WR_REG(0xcb); 
	LCD_WR_DATA(0x39);
	LCD_WR_DATA(0x2c);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x02);

	LCD_WR_REG(0xea); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0xe8); 
	LCD_WR_DATA(0x85);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x79);

	LCD_WR_REG(0xC0); //Power control
	LCD_WR_DATA(0x23); //VRH[5:0]

	LCD_WR_REG(0xC1); //Power control
	LCD_WR_DATA(0x11); //SAP[2:0];BT[3:0]

	LCD_WR_REG(0xC2);
	LCD_WR_DATA(0x11);

	LCD_WR_REG(0xC5); //VCM control
	LCD_WR_DATA(0x3d);
	LCD_WR_DATA(0x30);

	LCD_WR_REG(0xc7); 
	LCD_WR_DATA(0xaa);

	LCD_WR_REG(0x3A); 
	LCD_WR_DATA(0x55);

	LCD_WR_REG(0x36); // Memory Access Control
	LCD_WR_DATA(0x08);

	LCD_WR_REG(0xB1); // Frame Rate Control
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x11);

	LCD_WR_REG(0xB6); // Display Function Control
	LCD_WR_DATA(0x0a);
	LCD_WR_DATA(0xa2);

	LCD_WR_REG(0xF2); // 3Gamma Function Disable
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0xF7);
	LCD_WR_DATA(0x20);

	LCD_WR_REG(0xF1);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x30);

	LCD_WR_REG(0x26); //Gamma curve selected
	LCD_WR_DATA(0x01);

	LCD_WR_REG(0xE0); //Set Gamma
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x3f);
	LCD_WR_DATA(0x2f);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x0a);
	LCD_WR_DATA(0x53);
	LCD_WR_DATA(0xd5);
	LCD_WR_DATA(0x40);
	LCD_WR_DATA(0x0a);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0xE1); //Set Gamma
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x2c);
	LCD_WR_DATA(0xa2);
	LCD_WR_DATA(0x3f);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x0e);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x3c);
	LCD_WR_DATA(0x0F);
	LCD_WR_REG(0x11); //Exit Sleep
	delay_ms(80);//此处必须要延时80ms或者更多
	LCD_WR_REG(0x29); //display on
	LCD_Display_Dir(0);//初始化竖屏
}
//清屏函数
//Color:要清屏的填充色
void LCD_Clear(u16 Color)
{
	u16 i,j;
	Address_set(0,0,lcddev.width-1,lcddev.height-1);
    for(i=0;i<lcddev.width;i++)
	 {
	  for (j=0;j<lcddev.height;j++)
	   	{
        	 LCD_WR_DATA8(Color);
	    }

	  }
}
//显示一个16x16的汉字
//x,y:起点坐标
//c[3]为要显示的汉字
//Char_Color：字的颜色			
//Back_Color：背景颜色
void putGB(u8 x, u16 y,u8 c[3],u8 size)
{
	u8 temp;
    u8 num,pos,t,k;
	u16 x0=x;   
    u8 csize=(size/8+((size%8)?1:0))*size;		//得到字体一个字符对应点阵集所占的字节数
	
	for(k = 0;k < 100;k++)
	{
		if(size == 16)
		{
			if (strcmp(codeGB_16[k].Index,c) == 0)
			{
				num = k;
				break;
			}
		}
		else if(size == 24)
		{
			if (strcmp(codeGB_24[k].Index,c) == 0)
			{
				num = k;
				break;
			}
		}
		else if(size == 32)
		{
			if(strcmp(codeGB_32[k].Index,c) ==0)
			{
				num = k;
				break;
			}
		}
	}
	for(t = 0;t < csize;t++)
	{
		if(size == 16) temp = codeGB_16[num].Msk[t];
		else if(size == 24) temp = codeGB_24[num].Msk[t];
		else if(size == 32) temp = codeGB_32[num].Msk[t];
		for(pos = 0;pos < 8;pos++)
		{
			if(temp&0x80)DrawPoint(x,y,POINT_COLOR);
			else DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			x++;
			if(y>=lcddev.height)return;		//超区域了
			if((x-x0)==size)
			{
				x=x0;
				y++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}
	}
  
}

//显示多个岛鹤?
//x,y:起点坐标
//p[100]为要显示的汉字
//len：汉字的个数
//Char_Color：字的颜色			
//Back_Color：背景颜色
void putxGB(u8 x, u16 y,const u8 p[100],u8 size)
{
	u8 i=0,c[3];
	i = 0;
	while(p[i]!=  NULL)
	//for(i=0;i<len;i++)
	{
		c[0]=p[0+i];
		c[1]=p[1+i];
		if((c[0] > ' ' || c[0] == ' ') && (c[0] < '~' || c[0] == '~'))
		{
			LCD_ShowChar(x+size*i/2,y,c[0],size,0);
			i+=1;
		}
		else
		{
			putGB(x+size*i/2,y,c,size);
			i+=2;
		}
	}
}
//画点
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	Address_set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(POINT_COLOR); 	    
} 	 
void DrawPoint(u16 x,u16 y,u16 color)
{
	Address_set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(color); 	    
} 
//画一个大点
//POINT_COLOR:此点的颜色
void LCD_DrawPoint_big(u16 x,u16 y)
{
	LCD_Fill(x-1,y-1,x+1,y+1,POINT_COLOR);
} 
//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	Address_set(xsta,ysta,xend,yend);      //设置光标位置 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//设置光标位置 	    
	} 					  	    
}  

 void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)//画线
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_y=y2-y1; 
	delta_x=x2-x1; //计算坐标增量 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		DrawPoint(uRow,uCol,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_y=y2-y1; 
	delta_x=x2-x1; //计算坐标增量 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点
//		LCD_DrawPoint_big(uRow,uCol);//画点		
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   

void LCD_DrawLine_Big(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_y=y2-y1; 
	delta_x=x2-x1; //计算坐标增量 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
//		LCD_DrawPoint(uRow,uCol);//画点
		LCD_DrawPoint_big(uRow,uCol);//画点		
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
//在指定位置显示一个字符
//num:要显示的字符:" "--->"~"
//mode:叠加方式(1)还是非叠加方式(0)
//在指定位置显示一个字符
//num:要显示的字符:" "--->"~"
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
    u8 temp;
    u8 pos,t;
		u16 y0=y;   
    u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
		num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
		for(t=0;t<csize;t++)
		{   
			if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
			else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
			else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
			else if(size==32)temp=asc2_3216[num][t];
			else return;								//没有的字库
			for(pos=0;pos<8;pos++)
			{			    
				if(temp&0x80)DrawPoint(x,y,POINT_COLOR);
				else if(mode==0)DrawPoint(x,y,BACK_COLOR);
				temp<<=1;
				y++;
				if(y>=lcddev.height)return;		//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width)return;	//超区域了
					break;
				}
			}  	 
		}  	      	   	 	  
}    
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//color:颜色
//num:数值(0~4294967295);	
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,mode);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+48,size,mode); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{         	
	u8 t,temp;
//	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode); 
	}
} 
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//size 字体
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowString(u16 x,u16 y,const u8 *p,u8 size,u8 mode)
{         
    while(*p!='\0')
    {       
        if(x>lcddev.width-16){x=0;y+=16;}
        if(y>lcddev.height-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,size,mode);
        x+=(size/2);
        p++;
    }  
}

// X
// Y
// 数据
//  小数点左边位数
//小数点右边位数
//  字符大小
void LCD_Show_FloatNum16(u16 x,u16 y,double num,u8 lenth_int,u8 lenth_float,u8 size)
{
  char temp[15]="0";	 
  sprintf(temp,"%*.*f",lenth_float+lenth_int+1,lenth_float,num);	  
  LCD_ShowString(x,y,(u8 *)temp,size,0);		  
}










