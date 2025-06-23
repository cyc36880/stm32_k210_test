#ifndef _oled_h
#define _oled_h

#include "menu.h"

//sbit OLED_CS=P1^4; //片选
//sbit OLED_RST =P1^2;//复位
//sbit OLED_DC =P1^3;//数据/命令控制
//sbit OLED_SCL=P1^0;//时钟 D0（SCLK?
//sbit OLED_SDIN=P1^1;//D1（MOSI） 数据


#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)



//初始化   		  					   
void OLED_Init(void); 
//刷新整个屏幕
void disp_flush(void);


#endif







