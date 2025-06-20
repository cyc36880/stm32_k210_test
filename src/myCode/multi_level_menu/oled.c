#include "oled.h"


#define  u8 unsigned char 
#define	 u16 unsigned short int
#define  u32 unsigned int 
	
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据



/*............延时..............*/
static void delay_ms(unsigned int ms)
{                         
	HAL_Delay(ms);
}
/*.........命令与数据..........*/
static void OLED_WR_Byte(u8 dat,u8 cmd)    
{	
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
//	OLED_CS_Clr();

	HAL_SPI_Transmit_DMA(&hspi1, &dat, 1); // 硬件 DMA SPI
	
//	OLED_CS_Set();
	OLED_DC_Set();   	  
} 
static void OLED_Set_Pos(unsigned char x, unsigned char y)    //坐标位置
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
} 
static void OLED_Clear()             //清屏
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}



//刷新整个屏幕
void disp_flush(void)
{
//	u16 i, j;
//	for(i = 0; i < ScreenPara.screenhigh; i++)
//	{
//		OLED_Set_Pos(0,i);
//		for(j = 0; j < SCREENWIDTH; j++)
//		{
//			if(SCREENSHOWMANNER == ScreenNormal) { //正常显示
//				OLED_WR_Byte(DisplayBuff[i*SCREENWIDTH + j], OLED_DATA);
//			}
//			else if(SCREENSHOWMANNER == ScreenRollback) { //反转显示
//				OLED_WR_Byte(~DisplayBuff[i*SCREENWIDTH + j], OLED_DATA);
//			}
//		}
//	}
	//改变了oled初始化的默认值为0x00（逐行，可以自动回到下一行开头），原来为0x02（仅逐行，自动回到本行开头）
	OLED_Set_Pos(0,0); //避免刷新错位
	OLED_DC_Set();
//	OLED_CS_Clr();
	// HAL_SPI_Transmit_DMA(&hspi1, DisplayBuff, SCREENWIDTH*ScreenPara.screenhigh); // 硬件 DMA SPI
	HAL_SPI_Transmit(&hspi1, DisplayBuff, SCREENWIDTH*ScreenPara.screenhigh, HAL_MAX_DELAY); //无DMA，可避免刷新过快时的屏幕异常闪烁
}


void OLED_Init(void)     //初始化
{
  	OLED_RST_Set();
	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x00,OLED_CMD);// 默认为0x02
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}  

