#ifndef _menuconfig_h_
#define _menuconfig_h_



/************************
	include from outside
************************/

#include "main.h" //HAL函数库
#include "spi.h" //HAL函数库






/************************
	extern from outside
************************/

#define key_1_status HAL_GPIO_ReadPin(key_1_GPIO_Port, key_1_Pin)
#define key_2_status HAL_GPIO_ReadPin(key_2_GPIO_Port, key_2_Pin)

#define LED_GREEN(x) (TIM2->CCR2 = (x))
#define LED_RED(x)   (TIM2->CCR1 = (x))



/************************
	extern from interior
************************/
#include "menu.h"


//功能：初始化菜单相关内容
void MenuInit(void);




#endif




