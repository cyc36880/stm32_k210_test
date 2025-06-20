#ifndef _menuconfig_h_
#define _menuconfig_h_



/************************
	include from outside
************************/

#include "main.h" //HAL������
#include "spi.h" //HAL������






/************************
	extern from outside
************************/

extern uint32_t ADCarray[];





/************************
	extern from interior
************************/
#include "menu.h"


//���ܣ���ʼ���˵��������
void MenuInit(void);




#endif




