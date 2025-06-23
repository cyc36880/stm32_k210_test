#ifndef _oled_h
#define _oled_h

#include "menu.h"

//sbit OLED_CS=P1^4; //Ƭѡ
//sbit OLED_RST =P1^2;//��λ
//sbit OLED_DC =P1^3;//����/�������
//sbit OLED_SCL=P1^0;//ʱ�� D0��SCLK?
//sbit OLED_SDIN=P1^1;//D1��MOSI�� ����


#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)



//��ʼ��   		  					   
void OLED_Init(void); 
//ˢ��������Ļ
void disp_flush(void);


#endif







