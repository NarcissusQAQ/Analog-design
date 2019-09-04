#include "touch.h" 
#include "lcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
//#include "stmflash.h"

u16 buff[200][2],buff_length=0;
//u16 touch_touch=0;
_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0,
	0,
	0,
	
	0,
	
	0, 
	0,
	0,
	0,
	
	0,	  	 			  	 		
};					
//Ĭ��Ϊtouchtype=0������.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;
 	 			    			
//SPIд����
//������ICд��1byte����    
//num:Ҫд�������
void TP_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 	 
		TCLK=1;		//��������Ч	        
	}		 			    
} 		 
//SPI������ 
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������	   
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//������ʱ�� 	 
	TDIN=0; 	//����������
	TCS=0; 		//ѡ�д�����IC
	TP_Write_Byte(CMD);//����������
	delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		//��1��ʱ�ӣ����BUSY	    	    
	TCLK=0; 	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//�½�����Ч  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	TCS=1;		//�ͷ�Ƭѡ	 
	return(Num);   
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 15 	//��ȡ����
#define LOST_VAL 5	  	//����ֵ
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}
//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  
//////////////////////////////////////////////////////////////////////////////////		  
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6);//������Ȧ
}	  
//��һ�����(2*2�ĵ�)		   
//x,y:����
//color:��ɫ
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}

void TP_Draw_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x-1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x,y-1);
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
}

void paint(void)
{
	u8 i;
	if(TP_Scan(0)==0x80)
	{
		delay_ms(5);
		if(TP_Scan(0)==0x80)
		{
			buff[buff_length][0]=tp_dev.x0;
			buff[buff_length][1]=tp_dev.y0;
			for(i=0;i<buff_length;i++)
				LCD_DrawLine(buff[i][0]+5,buff[i][1]+5,buff[i+1][0]+5,buff[i+1][1]+5);
			buff_length++;
		}
	}
	else
	{
		buff_length=0;
	}
}


//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//�а�������
	{
		if(tp)TP_Read_XY2(&tp_dev.x0,&tp_dev.y0);//��ȡ��������
		else if(TP_Read_XY2(&tp_dev.x0,&tp_dev.y0))//��ȡ��Ļ����
		{
	 		tp_dev.x0=tp_dev.xoff-tp_dev.xfac*tp_dev.x0;//�����ת��Ϊ��Ļ����
			tp_dev.y0=tp_dev.yoff-tp_dev.yfac*tp_dev.y0;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
 			tp_dev.x0=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
 			tp_dev.y0=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}
		else//֮ǰ��û�б�����
		{
// 			tp_dev.x=0;
// 			tp_dev.y=0;
			tp_dev.x0=0;
			tp_dev.y0=0;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}	  

			  
//��ʾУ׼���(��������)
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,"x1:",16,0);
 	LCD_ShowString(40+80,160,"y1:",16,0);
 	LCD_ShowString(40,180,"x2:",16,0);
 	LCD_ShowString(40+80,180,"y2:",16,0);
	LCD_ShowString(40,200,"x3:",16,0);
 	LCD_ShowString(40+80,200,"y3:",16,0);
	LCD_ShowString(40,220,"x4:",16,0);
 	LCD_ShowString(40+80,220,"y4:",16,0);  
 	LCD_ShowString(40,240,"fac is:",16,0);     
	LCD_ShowNum(40+24,160,x0,4,16,0);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,160,y0,4,16,0);	//��ʾ��ֵ
	LCD_ShowNum(40+24,180,x1,4,16,0);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,180,y1,4,16,0);	//��ʾ��ֵ
	LCD_ShowNum(40+24,200,x2,4,16,0);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,200,y2,4,16,0);	//��ʾ��ֵ
	LCD_ShowNum(40+24,220,x3,4,16,0);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,220,y3,4,16,0);	//��ʾ��ֵ
 	LCD_ShowNum(40+56,240,fac,3,16,0); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.

}
		 
//������У׼����
//�õ��ĸ�У׼����
void TP_Adjust(void)
{								 
	u32 a0,b0,c0,d0,a,b,c,d;
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	
	u16 outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =BLACK;
	LCD_Clear(BLACK);//����   
	POINT_COLOR=RED;//��ɫ 
	LCD_Clear(BLACK);//���� 	   
	POINT_COLOR=YELLOW;
//	touch_touch=0;
	TP_Drow_Touch_Point(20,20,RED);//����1 
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(1);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES )//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x0;
			pos_temp[cnt][1]=tp_dev.y0;
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,GREEN);				//�����1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width -20,20,GREEN);	//�����2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,GREEN);			//�����3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
					//������
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[0][0]-pos_temp[1][0]);//�õ�xfac		 
					tp_dev.xoff=(lcddev.width+tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[0][1]-pos_temp[2][1]);//�õ�yfac
					tp_dev.yoff=(lcddev.height+tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//������Ԥ����෴��.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
						LCD_ShowString(40,100,"TP Need readjust!",16,0);
						tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
						if(tp_dev.touchtype)//X,Y��������Ļ�෴
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y��������Ļ��ͬ
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(BLACK);//����
					LCD_ShowString(35,300,"Touch Screen Adjust OK!",16,0);//У�����
					TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����
					delay_ms(500);
					
					a0=tp_dev.xfac;
					LCD_ShowNum(50,10,a0,3,16,0);
					a=(tp_dev.xfac-a0)*1000000;
					LCD_ShowNum(110,10,a,6,16,0);
					
					b0=tp_dev.xoff;
					LCD_ShowNum(50,30,b0,3,16,0);
					b=(tp_dev.xoff-b0)*1000000;
					LCD_ShowNum(110,30,b,6,16,0);
					
					c0=tp_dev.yfac;
					LCD_ShowNum(50,50,c0,3,16,0);
					c=(tp_dev.yfac-c0)*1000000;
					LCD_ShowNum(110,50,c,6,16,0);
					
					d0=tp_dev.yoff;
					LCD_ShowNum(50,70,d0,3,16,0);
					d=(tp_dev.yoff-d0)*1000000;
					LCD_ShowNum(110,70,d,6,16,0);
			//		while(1);
//					touch_touch=1;
//					TP_Save_Adjdata(); 
					
					//����   
//					return 1;//У�����				 
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>300)
		{
			//delay_ms(1000);
 			LCD_Clear(BLACK);
//			TP_Get_Adjdata();
			break;
	 	} 
 	}
}
//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 TP_Init(void)
{	
	//ע��,ʱ��ʹ��֮��,��GPIO�Ĳ�������Ч
	//��������֮ǰ,����ʹ��ʱ��.����ʵ���������������
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PB,PF�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 // PE4~6�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);//����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	// PF0.PF1�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //	��������
	GPIO_Init(GPIOF, &GPIO_InitStructure);	  
	TP_Read_XY(&tp_dev.x0,&tp_dev.y0);//��һ�ζ�ȡ��ʼ��	 
	LCD_Clear(WHITE);//����
	if(lcddev.dir ==0)
	{
		tp_dev.xfac = -0.0655522794;
		tp_dev.yfac = 0.0878018215;
		tp_dev.xoff = 0xFFF0;
		tp_dev.yoff = 0x015E;
		CMD_RDX=0XD0;
		CMD_RDY=0X90;
	}
	else if(lcddev.dir == 1)
	{
		tp_dev.xfac = -0.0877192989;
		tp_dev.yfac = -0.063351281;
		tp_dev.xoff = 0xFFE2;
		tp_dev.yoff = 0xFFF8;
		CMD_RDX=0X90;
		CMD_RDY=0XD0;	 
	}
//	TP_Get_Adjdata();//��ȡ����Ĵ���У׼����
//	 if(touch_touch!=1)
//	{
//		TP_Adjust();
//	}
	return 1; 									 
}


//���败�������Ժ���
void rtp_test(void)
{ 	
//	tp_dev.xfac=-0.084140;	 
//	tp_dev.xoff=280;
//			
//	tp_dev.yfac=-0.914105;
//	tp_dev.yoff=426;
	

	
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{
				TP_Draw_Big_Point(tp_dev.x0,tp_dev.y0,RED);		//��ͼ	
		}
//		else delay_ms(10);	//û�а������µ�ʱ�� 	    
		
	}
}


#define FLASH_SAVE_ADDR  0X08070000		//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)

//void TP_Save_Adjdata(void)
//{
//	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&tp_dev.xfac,14);
//	STMFLASH_Write(FLASH_SAVE_ADDR+14,&touch_touch,2);
//}

//u8 TP_Get_Adjdata(void)
//{					  
////	u8 temp;
//	STMFLASH_Read(FLASH_SAVE_ADDR+14,&touch_touch,2);//��ȡ�����,���Ƿ�У׼���� 		 
//	if(touch_touch==1)//�������Ѿ�У׼����			   
// 	{ 
//		STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)&tp_dev.xfac,14);//��ȡ֮ǰ�����У׼���� 
//		if(tp_dev.touchtype)//X,Y��������Ļ�෴
//		{
//			CMD_RDX=0X90;
//			CMD_RDY=0XD0;	 
//		}else				   //X,Y��������Ļ��ͬ
//		{
//			CMD_RDX=0XD0;
//			CMD_RDY=0X90;	 
//		}		 
//		return 1;	 
//	}
//	return 0;
//}	 
//	
	


