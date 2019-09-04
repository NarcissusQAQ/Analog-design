
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
������Ի���:��Ƭ��STC12LE5A60S2,����33M  ��Ƭ��������ѹ3.3V
����Ĭ��IO���ӷ�ʽ��
�����ߣ�RS-P3^5;    WR-P3^6;   RD-P3^7;   CS-P1^0;   REST-P1^2;
������: DB0-DB7��������P0^0-P0^7;  DB8-DB15��������P2^0-P2^7;
�����������ӷ�ʽ��(��ʹ�ô����ɲ�����)
T_CLK-P1^7;  T_CS-P1^4;  T_DIN-P3^0;  T_OUT-P3^1;  T_IRQ-P3^4;
*/
u16 BACK_COLOR=WHITE, POINT_COLOR=BLACK;   //����ɫ������ɫ
u8 DFT_SCAN_DIR = L2R_U2D;


//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

void LCD_Writ_Bus(u16 data)   //��������д�뺯��
{
	GPIO_Write(GPIOD,data);
	LCD_WR=0;
	LCD_WR=1; 
}
void LCD_WR_DATA8(u16 data) //��������-8λ����
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

//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
// 	if(lcddev.dir==1)
//	{
//		u16 t;
//		t=Xpos;
//		Xpos=lcddev.height-1-Ypos;//������ʵ���ǵ�תx,y����
//		Ypos=t;
//	}
	Address_set(Xpos,Ypos,Xpos,Ypos);
} 

//����LCD���Զ�ɨ�跽��
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510��IC�Ѿ�ʵ�ʲ���	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
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

//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//����
	{
		lcddev.dir=0;	//����
		lcddev.width=240;
		lcddev.height=320;
		lcddev.wramcmd=0x2c;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B; 
		DFT_SCAN_DIR = L2R_U2D;
	}
	else 				//����
	{	  				
		lcddev.dir=1;	//����
		lcddev.width=320;
		lcddev.height=240;
		lcddev.wramcmd=0x2c;
		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;
		DFT_SCAN_DIR = L2R_D2U;
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}

void Lcd_Init(void)
{
	//GPIOD��ʼ��
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
	delay_ms(20);	//�˴���֤Ҫ��5ms���ϲ��ܱ�֤��λ�ȶ�,�Ƽ�20ms
	LCD_REST=1;	
	delay_ms(20);	//�˴���֤Ҫ��5ms���ϲ��ܱ�֤��λ�ȶ�,�Ƽ�20ms		
	LCD_CS =0;  //��Ƭѡʹ��

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
	delay_ms(80);//�˴�����Ҫ��ʱ80ms���߸���
	LCD_WR_REG(0x29); //display on
	LCD_Display_Dir(0);//��ʼ������
}
//��������
//Color:Ҫ���������ɫ
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
//��ʾһ��16x16�ĺ���
//x,y:�������
//c[3]ΪҪ��ʾ�ĺ���
//Char_Color���ֵ���ɫ			
//Back_Color��������ɫ
void putGB(u8 x, u16 y,u8 c[3],u8 size)
{
	u8 temp;
    u8 num,pos,t,k;
	u16 x0=x;   
    u8 csize=(size/8+((size%8)?1:0))*size;		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	
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
			if(y>=lcddev.height)return;		//��������
			if((x-x0)==size)
			{
				x=x0;
				y++;
				if(x>=lcddev.width)return;	//��������
				break;
			}
		}
	}
  
}

//��ʾ�������?
//x,y:�������
//p[100]ΪҪ��ʾ�ĺ���
//len�����ֵĸ���
//Char_Color���ֵ���ɫ			
//Back_Color��������ɫ
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
//����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(u16 x,u16 y)
{
	Address_set(x,y,x,y);//���ù��λ�� 
	LCD_WR_DATA(POINT_COLOR); 	    
} 	 
void DrawPoint(u16 x,u16 y,u16 color)
{
	Address_set(x,y,x,y);//���ù��λ�� 
	LCD_WR_DATA(color); 	    
} 
//��һ�����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint_big(u16 x,u16 y)
{
	LCD_Fill(x-1,y-1,x+1,y+1,POINT_COLOR);
} 
//��ָ�����������ָ����ɫ
//�����С:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	Address_set(xsta,ysta,xend,yend);      //���ù��λ�� 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//���ù��λ�� 	    
	} 					  	    
}  

 void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)//����
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_y=y2-y1; 
	delta_x=x2-x1; //������������ 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		DrawPoint(uRow,uCol,color);//���� 
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
//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_y=y2-y1; 
	delta_x=x2-x1; //������������ 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//����
//		LCD_DrawPoint_big(uRow,uCol);//����		
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
	delta_x=x2-x1; //������������ 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
//		LCD_DrawPoint(uRow,uCol);//����
		LCD_DrawPoint_big(uRow,uCol);//����		
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
//������
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
//��ָ��λ����ʾһ���ַ�
//num:Ҫ��ʾ���ַ�:" "--->"~"
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
//��ָ��λ����ʾһ���ַ�
//num:Ҫ��ʾ���ַ�:" "--->"~"
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
    u8 temp;
    u8 pos,t;
		u16 y0=y;   
    u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
		num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
		for(t=0;t<csize;t++)
		{   
			if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
			else if(size==16)temp=asc2_1608[num][t];	//����1608����
			else if(size==24)temp=asc2_2412[num][t];	//����2412����
			else if(size==32)temp=asc2_3216[num][t];
			else return;								//û�е��ֿ�
			for(pos=0;pos<8;pos++)
			{			    
				if(temp&0x80)DrawPoint(x,y,POINT_COLOR);
				else if(mode==0)DrawPoint(x,y,BACK_COLOR);
				temp<<=1;
				y++;
				if(y>=lcddev.height)return;		//��������
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width)return;	//��������
					break;
				}
			}  	 
		}  	      	   	 	  
}    
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//color:��ɫ
//num:��ֵ(0~4294967295);	
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
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
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:0,�ǵ�����ʾ;1,������ʾ.
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
//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//size ����
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
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
// ����
//  С�������λ��
//С�����ұ�λ��
//  �ַ���С
void LCD_Show_FloatNum16(u16 x,u16 y,double num,u8 lenth_int,u8 lenth_float,u8 size)
{
  char temp[15]="0";	 
  sprintf(temp,"%*.*f",lenth_float+lenth_int+1,lenth_float,num);	  
  LCD_ShowString(x,y,(u8 *)temp,size,0);		  
}










