#include "myMain.h"

#include "menu.h"
#include "IIC_Port.h"
#include "k210_check.h"

void setup(void)
{
    IIC_Port_Init();
    k210_iic_init();
	MenuInit(); //
}


void loop(void)
{
    MenuRun();
}
