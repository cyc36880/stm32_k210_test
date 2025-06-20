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

extern uint32_t ADCarray[];





/************************
	extern from interior
************************/
#include "menu.h"


//功能：初始化菜单相关内容
void MenuInit(void);




#endif




