#include "menuconfig.h"

#include "menu.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数
#include "menu_tool.h" //工具



static menu_area startMenu;
static menu_area mainMenu; //第一个菜单


static void menu_start(menu_area *target);
static void menu_main(menu_area *target);

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	AddToFunctionTicker(50, normalRun, MenuRefresh, 0); //定时刷新
	TargetMenuPointrt.style = ENABLE; //指示器动效
	
	TargetMenu = &startMenu;
	
	// 开机界面
	SetMenu(&startMenu, 0, 0, 0, 0, ENABLE, NULL)->menuinterface = menu_start;
	AddToMenuListInit(&startMenu);
	AddToSpecialFunction(&startMenu, MenuTime, 50);

	
	SetMenu(&mainMenu, 0, 0, 0, 0, ENABLE, NULL)->menuinterface = menu_main;
	AddToMenuListInit(&mainMenu);
}

/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
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
// ======================== 菜 单 固 定 函 数 =======================
// ==================================================================



//**************** while中全速运行  ************************


void AlwaysRun(void)
{
	
}



/* ************** 菜单跟随刷新 ******************** */


/**** 菜单的每次刷新都会执行该函数 执行优先级最高 《！！不要写显示相关函数，会被异常清空！！》 ****/
void MenuAlwaysRun_PH(void)
{

}


/**** 菜单的每次刷新都会执行该函数 执行优先级中****/
void MenuAlwaysRun_PM(void)
{

}



/**** 菜单的每次刷新都会执行该函数 执行优先级低 ****/
void MenuAlwaysRun_PL(void)
{

}




//************* 初始化菜单相关内容 *********************

void MenuInit(void)
{
	MenuSysBaseInit(); //系统初始化
	MakeMenu(); //菜单列表初始化
	sMenuListInit();
	/* ********** USER BEGIN********** */
	
	
	/* ********** USER END ********** */
	
	MenuHeartTimeStart = ENABLE;//菜单心跳
	
}








