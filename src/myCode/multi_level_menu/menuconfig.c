#include "menuconfig.h"

#include "menu.h"
#include "oledfont.h" //�����ļ�
#include "menufontshow.h" //�ַ�����
#include "Graphicalfunctions.h" // ͼ�λ�����
#include "menu_tool.h" //����


menu_area mainMenu; //��һ���˵�

static void menu_f(menu_area *target);

/*
	���ܣ���ʼ���˵��б� <�˺����������޸�>
*/
static void MakeMenu(void)
{
	AddToFunctionTicker(50, normalRun, MenuRefresh, 0); //��ʱˢ��
	TargetMenuPointrt.style = ENABLE; //ָʾ����Ч
	
	TargetMenu = &mainMenu;
	
	SetMenu(&mainMenu, 0, 20, 70, 20, ENABLE, NULL)->menuinterface = menu_f;

}

/*
	���ܣ��˵���ʾ�ľ���ʵ�ֺ���
	ע�⣺���������� void ���ͣ��βα����ҽ�Ϊ menu_area *target
*/


static void menu_f(menu_area *target)
{
	static uint8_t i=0;
	m_printf(target, 0, 0, "����123 %d", i++);
	
	

}








// ==================================================================
// ======================== �� �� �� �� �� �� =======================
// ==================================================================



//**************** while��ȫ������  ************************


void AlwaysRun(void)
{
	
}



/* ************** �˵�����ˢ�� ******************** */


/**** �˵���ÿ��ˢ�¶���ִ�иú��� ִ�����ȼ���� ��������Ҫд��ʾ��غ������ᱻ�쳣��գ����� ****/
void MenuAlwaysRun_PH(void)
{

}


/**** �˵���ÿ��ˢ�¶���ִ�иú��� ִ�����ȼ���****/
void MenuAlwaysRun_PM(void)
{

}



/**** �˵���ÿ��ˢ�¶���ִ�иú��� ִ�����ȼ��� ****/
void MenuAlwaysRun_PL(void)
{

}




//************* ��ʼ���˵�������� *********************

void MenuInit(void)
{
	MenuSysBaseInit(); //ϵͳ��ʼ��
	MakeMenu(); //�˵��б��ʼ��
	sMenuListInit();
	/* ********** USER BEGIN********** */
	
	
	/* ********** USER END ********** */
	
	MenuHeartTimeStart = ENABLE;//�˵�����
	
}








