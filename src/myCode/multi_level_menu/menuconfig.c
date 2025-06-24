#include "menuconfig.h"

#include "menu.h"
#include "oledfont.h" //�����ļ�
#include "menufontshow.h" //�ַ�����
#include "Graphicalfunctions.h" // ͼ�λ�����
#include "menu_tool.h" //����



static menu_area startMenu;
static menu_area mainMenu; //��һ���˵�


static void menu_start(menu_area *target);
static void menu_main(menu_area *target);

/*
	���ܣ���ʼ���˵��б� <�˺����������޸�>
*/
static void MakeMenu(void)
{
	AddToFunctionTicker(50, normalRun, MenuRefresh, 0); //��ʱˢ��
	TargetMenuPointrt.style = ENABLE; //ָʾ����Ч
	
	TargetMenu = &startMenu;
	
	// ��������
	SetMenu(&startMenu, 0, 0, 0, 0, ENABLE, NULL)->menuinterface = menu_start;
	AddToMenuListInit(&startMenu);
	AddToSpecialFunction(&startMenu, MenuTime, 50);

	
	SetMenu(&mainMenu, 0, 0, 0, 0, ENABLE, NULL)->menuinterface = menu_main;
	AddToMenuListInit(&mainMenu);
}

/*
	���ܣ��˵���ʾ�ľ���ʵ�ֺ���
	ע�⣺���������� void ���ͣ��βα����ҽ�Ϊ menu_area *target
*/

static void menu_start(menu_area *target)
{
	static const char *str = "K210 TEST";
	static uint8_t count=0;
    if (CkeckMenuList(target))
	{
		FontInfoType *font = StringDeal(str);
		uint8_t str_len_size = 
		target->x = MSCX(font->maxPix);
		target->y = MSCY(font->ascsize[1]);
		target->width = font->maxPix;
		target->high = font->ascsize[1];
		return;
	}
	if (TriggerCheck(target, MenuTime))
	{
		count++;
	}
	if (count == 30)
	{
		count=0;
		gotoMenu(&mainMenu, Menu_Sub);
	}

	m_printf(target, 0, 0, "%s", str);

}


static void menu_main(menu_area *target)
{
	static const char *title_str = "k210 TEST";
	static uint8_t title_str_size = 0;
	if (CkeckMenuList(target))
	{
		FontInfoType *font = StringDeal(title_str);
		title_str_size = font->maxPix;
		target->x = 0;
		target->y =0;
		target->width = SCREENWIDTH;
		target->high = font->ascsize[1];
		return;
	}
	m_printf(NULL, MSCX(title_str_size), TargetMenuPointrt.y, "%s", title_str);


	

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








