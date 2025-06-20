#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>  //uint16_t 等所在地
#include <stdlib.h>  //malloc   所在地
#include <stdbool.h> //bool     类型所在地

#include "oled.h"
#include "menuconfig.h"

/*
特别注意：

	请勿在中断中使用以下函数！！！
		
	每个函数详细的注意事项以及返回值和功能等，请跳转该函数上部查看
	
	该菜单部分使用malloc申请空间，无free，注意内存溢出。请确保heap(堆)大小足够
*/

// ================================ 菜 单 ==============================

#define MENUHEARDID 1 //菜单列表起始ID

#ifndef ENABLE
	#define ENABLE  1 //使能
#endif
#ifndef DISABLE
	#define DISABLE 0 //失能
#endif

// 菜单操作
enum MenuState
{
	Menu_noaction=0, // 无操作
	Menu_up,  		 // 向上
	Menu_down, 		 // 向下
	Menu_Sub,        // 子类
	Menu_Father,     // 父类
	Menu_confirm 	 // 确认
};

// 用户特殊信息
enum SpecialInformation
{
	MenuTime = 0x01, //时间列表
	EnterMenu = 0x02, // 进入菜单，if末尾建议加return 
	ExitMenu = 0x04,  // 退出菜单，if末尾建议加return
	EnterShowMenuList = 0x08, // 进入显示菜单列表，if末尾建议加return 滚动显示禁用该功能
	ExitShowMenuList = 0x10, // 退出显示菜单列表，if末尾建议加return  滚动显示禁用该功能
	
	NotIgnore = 0x20, //不在显示的范围，任然刷新。可注册，不可检查
	
	/*****上述功能的改进*****/
	
	MenuTimeForce = 0x40, //！！！时间列表强制执行。 与MenuTime配合使用，否则无效，无需对此判断，if末尾必须加return！！！
	/*
		↑↑↑ 注册时，不加该标志，则仅当菜单显示时才能正常计数。
		！！加入该标志，由中断调取相应函数，并且只要处于该菜单所处的列表中，就能正常计数，并调用相应函数
	*/
	
	/******** 其 它 **********/
	MenuHaveOverall = 0x80,//不可使用，其它功能占用该位。菜单列表全局
	MenuScrollingY  = 0x100,//不可使用，其它功能占用该位。菜单列表Y滚动
	MenuScrollingX  = 0x200,//不可使用，其它功能占用该位。菜单列表X滚动
};




//菜单时间队列
typedef struct MENU_TIMEMS
{
	uint16_t counttime; //起始计数值
	uint16_t timems;    //预设时间
}menu_timems;
//列表初始化
typedef struct LISTINIT
{
	void *Target;
	struct LISTINIT *next;
	uint8_t flag;
}ListInit;

//菜单参数
typedef struct MENU_AREA
{
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t high;
	uint16_t id; //菜单ID，不要修改
	bool checked; //能否被选中 
	bool menulistend; //菜单列表结束
	struct MENU_AREA *next;     //下一个
	struct MENU_AREA *previous; //上一个
	struct MENU_AREA *subclass; //子类
	struct MENU_AREA *father;   //父类
	void (*menuinterface)(struct MENU_AREA *target); //菜单内容
	menu_timems *menu_time;
	uint16_t specialfeatures;//特殊功能注册
	uint16_t specfeattrigflag; //特殊功能触发标记
	
	ListInit ListTarget; //初始化列表
}menu_area;


typedef struct MENUTARGET //菜单指示器
{
	menu_area * TargetMenuP;
	menu_area * LastTargetMenuP;
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t high;
	uint8_t R;  //圆角
	bool style; //动画
 	bool show;  //显示
}MenuTargetTypedef;

//列表全局队列
typedef struct MENULISTOVERALL 
{
	menu_area *Affiliation; //隶属于
	void (*menuinterface)(void); //菜单内容
	struct MENULISTOVERALL *next;
}MenuListOverall;





// ----------- 菜 单 寻 址 --------------------



//功能：对菜单上下偏移寻址 
menu_area *MenuListAddressing(menu_area *target, bool upordown, uint16_t offset);

//功能：对菜单头前后偏移寻址 
menu_area *MenuHeadAddressing(menu_area *target, bool upordown, uint16_t offset);

//功能：寻找以目标单元为基准的第num个可选中的菜单，无则返回原地址
menu_area *NextCancheMenuList(menu_area *target, int16_t num);

//功能：寻找以目标单元为基准的第num个可选中的菜单头，无则返回原地址
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num);

//功能：找到菜单所在菜单列表的菜单头，空指针NULL 
menu_area *FindMeunListHeard(menu_area *target);

//功能：找到菜单所在菜单列表的菜单尾，空指针NULL
menu_area *FindMeunListTail(menu_area *target);

//功能：返回当前显示列表的头，即使它不能被选中
menu_area *MenuListShowHead(menu_area *target);

//功能：返回当前显示列表的尾，即使它不能被选中
menu_area *MenuListShowTail(menu_area *target);

//功能：找到为ID的菜单  mod：0：无则返回原指针，1：无则返回NULL
menu_area *FindMenuOfID(menu_area *target, uint16_t id, bool mod);



// ---------- 菜 单 注 册 与 链 接 -----------------



//功能：注册或添加菜单 
menu_area *AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);
//功能：对已有菜单注册或添加菜单 
menu_area *SetMenu(menu_area *target, int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);


//功能：快速目标菜单下方仿制  	mod：类型 0:有超出部分立即按照头仿造 1:只有完全在屏幕下方才按头仿造 2:仅在最后下方仿造
menu_area *FastSimilarMenuDown(menu_area *target, menu_area *source, uint8_t mod);
//功能：快速目标菜单右侧仿制  	mod：类型 0:有超出部分立即按照头仿造 1:只有完全在屏幕右方才按头仿造 2:仅在最后右方仿造
menu_area *FastSimilarMenuRight(menu_area *target, menu_area *source, uint8_t mod);


//功能：批量初始化  source：menu_area类型的数组  n：数组个数  mod：类型  menuinterface：统一链接的函数
void BatchFastSimilarMenuDown(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target));
//功能：批量初始化  source：menu_area类型的数组  n：数组个数  mod：类型  menuinterface：统一链接的函数
void BatchFastSimilarMenuRight(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target));


//功能：链接到父类
void LinkToParentClass(menu_area *father, menu_area *sub);


//功能：目标菜单首尾相连，空指针跳过
void MakeMenuListRing(menu_area *target);



// --------------- 初 始 化 列 表 -------------


// 功能：向列表加入初始化执行函数
ListInit* AddToListInit(ListInit **ListTarget, ListInit *target, void *func);

//功能：列表检查  注意：if 末尾加  return！！
uint8_t CheckListInit(ListInit *ListTarget);


// ----- 菜 单 列 表 ---


//功能：加入到菜单初始化列表中
void AddToMenuListInit(menu_area *target);

//功能：开始菜单列表初始化
void sMenuListInit(void);

//功能：菜单列表检查    注意：if 末尾加  return！！
uint8_t CkeckMenuList(menu_area *target);




// ************* 其 他 功 能 *******************



//功能：菜单列表始终执行函数 <仅对当前列表>
MenuListOverall *MenuOverall(menu_area *target);


//功能：对已有的时间添加至菜单 <仅针对时间的特殊功能注册>
void SetMenuTime(menu_area *target, menu_timems *menutime_obj, uint16_t function, uint16_t ms);

//功能：特殊功能注册
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms);

//功能：特殊功能检查，触发返回1，否则返回0
bool TriggerCheck(menu_area *target, enum SpecialInformation function);


//功能：手动指定要改变的菜单，并广播消息 target：指定的菜单指针 mes：消息
void gotoMenu(menu_area *target, enum MenuState mes);




extern MenuTargetTypedef TargetMenuPointrt;// 实时目标菜单
#define TargetMenu TargetMenuPointrt.TargetMenuP

extern enum MenuState StatusInformation; //输入设备状态
extern enum MenuState StatusInformationAlways; // 输入设备状态 <不会改变>

extern uint8_t InuptEnable; //输入使能
extern uint8_t ResponseEnable; //菜单切换响应




/*
* 输入要显示占用的高度（high）或宽度（width），输出左上角的x或y相对于
* 菜单中心的坐标
*/
#define MCX(target, w) ((target->width - (w)) / 2)
#define MCY(target, h) ((target->high - (h)) / 2)



void MenuAlwaysRun_PH(void);
void MenuAlwaysRun_PM(void);
void MenuAlwaysRun_PL(void);
void MenuSysBaseInit(void); //菜单系统初始化
void AlwaysRun(void);









// =========================== 屏 幕 ====================================


#define SCREENWIDTH 128 // 屏幕宽度
#define SCREENHIGH  64  // 屏幕高度

//向上整除 x=8 -> 1, x=9 -> 2
#define DIVIDEUP(x) (x*10/8%10 ? x/8+1:x/8)

enum ScreenShowManner //屏幕显示方式
{
	ScreenNormal,  //正常
	ScreenRollback //反转
};

enum GraphicsShowManner //图形显示方式
{
	GraphicsNormal,  //正常
	GraphicsColless, //无色
	GraphicsCover,   //覆盖
	GraphicsRollColor //反色（读取目标点，取反）
};

//屏幕参数
typedef struct
{
	const uint16_t screenwidth; //屏幕宽度
	const uint16_t screenhigh;  //屏幕高度 (向上除8)
	uint8_t refresh; //刷新标志
}TypedefScreen;

extern enum ScreenShowManner SCREENSHOWMANNER; //屏幕显示方式
extern enum GraphicsShowManner GRAPHICSSHOWMANNER;//图形显示方式

extern unsigned char DisplayBuff[]; // 屏幕显示缓存
extern TypedefScreen ScreenPara; // 屏幕具体参数

//功能：屏幕多次刷新注册  mod  0：依附于本有的刷新次数，无则创建  其它：创建N次刷新
void MenuRefresh(uint32_t mod);
// 功能：在目标菜单的相对位置画点
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b); 
//清空显示缓存
void ClearnBuff(void);


/*
	功能：判断菜单是否在屏幕上

	ret：0不在 1在
*/
uint8_t inScreen(menu_area *target);


/*
* 输入要显示占用的高度（high）或宽度（width），输出左上角的x或y相对于
* 屏幕中心的坐标
*/
#define MSCX(width) ((SCREENWIDTH - (width)) / 2)
#define MSCY(high)  ((SCREENHIGH - (high)) / 2)







// ============================== 其 它 ==================================


/*
	功能：菜单申请空间
	size：申请空间的字节数
	@ret：申请空间的首地址，失败为空
*/
void *MenuMalloc(uint16_t size);

extern uint32_t MenuMallocSize;//菜单申请的空间大小


// 功能：菜单运行函数，为保证正常运行，该函数在while中每秒循环次数应大于2000次
void MenuRun(void); 
// 功能：菜单心跳执行，每1ms执行该函数
//ms 间隔几毫秒
void MenuTicker_ms(uint16_t ms);
//菜单心跳开始标志 放在最后 置ENABLE运行
extern bool MenuHeartTimeStart; 




#endif





