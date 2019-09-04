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
//默认为touchtype=0的数据.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;
 	 			    			
//SPI写数据
//向触摸屏IC写入1byte数据    
//num:要写入的数据
void TP_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 	 
		TCLK=1;		//上升沿有效	        
	}		 			    
} 		 
//SPI读数据 
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据	   
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//先拉低时钟 	 
	TDIN=0; 	//拉低数据线
	TCS=0; 		//选中触摸屏IC
	TP_Write_Byte(CMD);//发送命令字
	delay_us(6);//ADS7846的转换时间最长为6us
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		//给1个时钟，清除BUSY	    	    
	TCLK=0; 	     	    
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//下降沿有效  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//只有高12位有效.
	TCS=1;		//释放片选	 
	return(Num);   
}
//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 15 	//读取次数
#define LOST_VAL 5	  	//丢弃值
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
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
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  
//////////////////////////////////////////////////////////////////////////////////		  
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6);//画中心圈
}	  
//画一个大点(2*2的点)		   
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//中心点 
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
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//有按键按下
	{
		if(tp)TP_Read_XY2(&tp_dev.x0,&tp_dev.y0);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x0,&tp_dev.y0))//读取屏幕坐标
		{
	 		tp_dev.x0=tp_dev.xoff-tp_dev.xfac*tp_dev.x0;//将结果转换为屏幕坐标
			tp_dev.y0=tp_dev.yoff-tp_dev.yfac*tp_dev.y0;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
 			tp_dev.x0=tp_dev.x[0];//记录第一次按下时的坐标
 			tp_dev.y0=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}
		else//之前就没有被按下
		{
// 			tp_dev.x=0;
// 			tp_dev.y=0;
			tp_dev.x0=0;
			tp_dev.y0=0;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	  

			  
//提示校准结果(各个参数)
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
	LCD_ShowNum(40+24,160,x0,4,16,0);		//显示数值
	LCD_ShowNum(40+24+80,160,y0,4,16,0);	//显示数值
	LCD_ShowNum(40+24,180,x1,4,16,0);		//显示数值
	LCD_ShowNum(40+24+80,180,y1,4,16,0);	//显示数值
	LCD_ShowNum(40+24,200,x2,4,16,0);		//显示数值
	LCD_ShowNum(40+24+80,200,y2,4,16,0);	//显示数值
	LCD_ShowNum(40+24,220,x3,4,16,0);		//显示数值
	LCD_ShowNum(40+24+80,220,y3,4,16,0);	//显示数值
 	LCD_ShowNum(40+56,240,fac,3,16,0); 	//显示数值,该数值必须在95~105范围之内.

}
		 
//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{								 
	u32 a0,b0,c0,d0,a,b,c,d;
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	
	u16 outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =BLACK;
	LCD_Clear(BLACK);//清屏   
	POINT_COLOR=RED;//红色 
	LCD_Clear(BLACK);//清屏 	   
	POINT_COLOR=YELLOW;
//	touch_touch=0;
	TP_Drow_Touch_Point(20,20,RED);//画点1 
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES )//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x0;
			pos_temp[cnt][1]=tp_dev.y0;
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,GREEN);				//清除点1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width -20,20,GREEN);	//清除点2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,GREEN);			//清除点3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[0][0]-pos_temp[1][0]);//得到xfac		 
					tp_dev.xoff=(lcddev.width+tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[0][1]-pos_temp[2][1]);//得到yfac
					tp_dev.yoff=(lcddev.height+tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,GREEN);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
						LCD_ShowString(40,100,"TP Need readjust!",16,0);
						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
						if(tp_dev.touchtype)//X,Y方向与屏幕相反
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y方向与屏幕相同
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(BLACK);//清屏
					LCD_ShowString(35,300,"Touch Screen Adjust OK!",16,0);//校正完成
					TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据
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
					
					//清屏   
//					return 1;//校正完成				 
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
//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
u8 TP_Init(void)
{	
	//注意,时钟使能之后,对GPIO的操作才有效
	//所以上拉之前,必须使能时钟.才能实现真正的上拉输出
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF, ENABLE);	 //使能PB,PF端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 // PE4~6端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);//上拉

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	// PF0.PF1端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //	上拉输入
	GPIO_Init(GPIOF, &GPIO_InitStructure);	  
	TP_Read_XY(&tp_dev.x0,&tp_dev.y0);//第一次读取初始化	 
	LCD_Clear(WHITE);//清屏
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
//	TP_Get_Adjdata();//读取保存的触屏校准参数
//	 if(touch_touch!=1)
//	{
//		TP_Adjust();
//	}
	return 1; 									 
}


//电阻触摸屏测试函数
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
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{
				TP_Draw_Big_Point(tp_dev.x0,tp_dev.y0,RED);		//画图	
		}
//		else delay_ms(10);	//没有按键按下的时候 	    
		
	}
}


#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

//void TP_Save_Adjdata(void)
//{
//	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&tp_dev.xfac,14);
//	STMFLASH_Write(FLASH_SAVE_ADDR+14,&touch_touch,2);
//}

//u8 TP_Get_Adjdata(void)
//{					  
////	u8 temp;
//	STMFLASH_Read(FLASH_SAVE_ADDR+14,&touch_touch,2);//读取标记字,看是否校准过！ 		 
//	if(touch_touch==1)//触摸屏已经校准过了			   
// 	{ 
//		STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)&tp_dev.xfac,14);//读取之前保存的校准数据 
//		if(tp_dev.touchtype)//X,Y方向与屏幕相反
//		{
//			CMD_RDX=0X90;
//			CMD_RDY=0XD0;	 
//		}else				   //X,Y方向与屏幕相同
//		{
//			CMD_RDX=0XD0;
//			CMD_RDY=0X90;	 
//		}		 
//		return 1;	 
//	}
//	return 0;
//}	 
//	
	


