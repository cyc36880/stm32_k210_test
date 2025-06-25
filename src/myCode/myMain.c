#include "myMain.h"

#include "menu.h"
#include "IIC_Port.h"

void setup(void)
{
    IIC_Port_Init();
	MenuInit(); //
}


void loop(void)
{
    MenuRun();
}
