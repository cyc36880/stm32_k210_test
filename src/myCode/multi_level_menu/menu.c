#include "menu.h"
#include "Graphicalfunctions.h"
#include "menu_tool.h"
#include "menufontshow.h"
/*
	0: 输入立即触发
	1: 输入回弹后再触发，但不影响StatusInformationAlways
*/
#define SCAN_MOD 1

// 0: 返回至菜单头的父类   1: 返回至当前菜单的父类
#define MENU_FATHER 0


void FunctionTickerRun(void); //在while中查询函数是否执行
void FunctionTickerRunIRQ(uint16_t ms); //在中断中查询函数是否该执行
static void MenuRest(void); //单个菜单刷新后的复位

// 输入禁止时
static void StopKeyReceive(uint8_t *p);

static KeyTypedef MenuKey; //按键 多击与长按
static enum MenuState MesSIF = Menu_noaction; //用于消息，记录输入状态

// ================================= 设 备 ========================================

enum MenuState StatusInformation = Menu_noaction; //输入设备状态
enum MenuState StatusInformationAlways = Menu_noaction; // 输入设备状态 <不会改变>


uint8_t InuptEnable = ENABLE; //输入响应
uint8_t ResponseEnable = ENABLE; //菜单切换响应

/*
	* 功能：输入设备状态
*/

static enum MenuState Scan(void)
{
	static enum MenuState adcstate = Menu_noaction;

	
	return adcstate;
}

/*
	按键状态获取与简单的处理
	函数中的 return 1 会注册刷新， 在无定时刷新时有效，定时刷新无明显效果
*/

static bool KeyState(enum MenuState *k)
{
	uint8_t keystate=0;
	uint8_t keycount=0;
	
	static enum MenuState KeySt = Menu_noaction;
	// 通用按键处理
	if(KeySt != Scan())
	{
		HAL_Delay(1);
		if(KeySt != Scan())
		{
			#if SCAN_MOD
				*k = KeySt;
				KeySt = Scan();
				StatusInformationAlways = KeySt;
			#else
				KeySt = Scan();
				*k = KeySt;
				StatusInformationAlways = KeySt;
			#endif
			return 1;
		}
	}
	// 按键附加功能处理 -- 多击与长按
	if(StatusInformationAlways >= Menu_up && StatusInformationAlways <= Menu_Father) 
		MenuKey.ReadKey = 1;
	else 
		MenuKey.ReadKey = 0;
	
	if(GetKeyState(&MenuKey, &keystate, &keycount ))
	{
		if(keystate == 1) { //多击 keycount为多击后的次数
		}
		if(keystate == 2) { //长按
			*k = StatusInformationAlways;
		}
		return 1;
	}
	
	return 0;
}


/*
	功能：输入设备扫描，该函数仅由系统调用
*/
static void EquipmentState(void)
{
	if(InuptEnable == DISABLE) { //输入是否使能
		StopKeyReceive(&InuptEnable);
		return;
	}
	if(KeyState(&StatusInformation)){ //按键扫描
		if(StatusInformation != Menu_noaction){ //非空闲
//			MenuRefresh(0); //屏幕刷新
		}
	}
}


// ================================= 屏 幕 ========================================

const unsigned int BUFFWEIGH = SCREENWIDTH;
const unsigned int BUFFHIGH  = DIVIDEUP(SCREENHIGH);
enum ScreenShowManner SCREENSHOWMANNER = ScreenNormal; //屏幕显示方式
enum GraphicsShowManner GRAPHICSSHOWMANNER = GraphicsNormal; //图形显示方式

// 定义屏幕信息
TypedefScreen ScreenPara = {BUFFWEIGH, BUFFHIGH, 1};

// 显示缓存
unsigned char DisplayBuff[BUFFWEIGH * BUFFHIGH] = {0};


//清空显示缓存
void ClearnBuff(void)
{
	unsigned int i, j = BUFFWEIGH * BUFFHIGH;
	for(i=0; i<j; i++)
		DisplayBuff[i] = 0;
}
/*
	功能：屏幕多次刷新注册
	mod：  0：依附于本有的刷新次数，无则创建  其它：创建N次刷新
*/
void MenuRefresh(uint32_t mod)
{
	if(mod==0) {
		if(ScreenPara.refresh == 0) {
			ScreenPara.refresh += 1;
		}
	}
	else {
		ScreenPara.refresh += mod;
	}
}

/*
	功能：判断菜单是否在屏幕上

	ret：0不在 1在
*/
uint8_t inScreen(menu_area *target)
{
	if(target->x>=SCREENWIDTH || target->y>=SCREENHIGH)
	{
		return 0;
	}
	else if(target->x>=0 && target->y>=0)
	{
		return 1;
	}
	else if( (target->x+target->width)<=0 || (target->y+target->high)<=0)
	{
		return 0;
	}
	return 1;
}



// ========================== 菜 单 ==================================

uint32_t MenuMallocSize = 0;//菜单申请的空间大小
/*
	功能：菜单申请空间
	size：申请空间的字节数
	@ret：申请空间的首地址，失败为空
*/
void *MenuMalloc(uint16_t size)
{
	void *addr;
	addr = malloc(size);
	if(addr) {
		MenuMallocSize += size;
	}
	return addr;
}


/*
*	功能：对已有菜单注册或添加菜单 
*	target：已有的菜单
*	x, y, width, high: 为注册菜单单元的位置与大小 
*	checked: 能否被选中，1能 0否 
*	transfer：为 NULL 注册的是菜单头，填入菜单地址为其尾加入 
*/
menu_area *SetMenu(menu_area *target, int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p = target;
	menu_area *k;
	
	if(!p) return NULL;
	
	ClearnMemory(target, sizeof(menu_area)); //空间清空
	
	if(transfer != NULL)  //尾部加入 
	{
		k = FindMeunListTail(transfer); //找到文件尾部
		k->next = p;
		p->previous = k;
		p->id = k->id + 1; 
	}
	else //创建菜单头
	{
		p->id = MENUHEARDID; // 菜单头ID 
		p->previous = NULL; //上一个
	}

	p->x = x;
	p->y = y;
	p->width = width;
	p->high = high;
	p->checked =  checked; // 能否选中 
	p->menulistend = DISABLE; //菜单列表结束
	p->next = NULL;      //下一个
	p->subclass = NULL;  //子类 
	p->father = NULL;    // 父类 
	p->menuinterface = NULL; // 菜单内容
	p->specialfeatures = NULL; //特殊功能
	p->specfeattrigflag = NULL;//特殊功能触发标志
	p->menu_time = NULL; //时间队列
	return p;
}

/*
*	功能：注册或添加菜单 
*	x, y, width, high: 为注册菜单单元的位置与大小 
*	checked: 能否被选中，1能 0否 
*	transfer：为 NULL 注册的是菜单头，填入菜单地址为其尾加入 
*/
menu_area * AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p;
	
	p = (menu_area *) MenuMalloc(sizeof(menu_area)); //申请内存空间
	
	return SetMenu(p, x, y, width, high, checked, transfer);
}

/*
	功能：快速目标菜单下方仿制
	target：要仿制的目标菜单
	source：拥有的菜单
	mod：类型 0:有超出部分立即按照头创建 1:只有完全在屏幕下方才按头创建 2:仅在最后下方仿造
*/

menu_area * FastSimilarMenuDown(menu_area *target, menu_area *source, uint8_t mod)
{
	if(!source) return NULL;
	if(!target) {
		return SetMenu(source, 0, 0, 0, 0, ENABLE, NULL);
	}
	target = FindMeunListHeard(target);
	menu_area *p = FindMeunListTail(target); 
	
	if(mod == 0)  { //超出屏幕，立即
		if(p->y+p->high*2 > SCREENHIGH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width,target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
		}
	}
	else if(mod == 1){
		if(p->y+p->high >= SCREENHIGH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width,target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
		}
	}
	else {
		return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
	}
}


/*
	功能：快速目标菜单右侧仿制
	target：要仿制的目标菜单
	source：拥有的菜单
	mod：类型 0:有超出部分立即按照头创建 1:只有完全在屏幕右侧才按头创建 2:仅在最后右侧仿造
*/

menu_area * FastSimilarMenuRight(menu_area *target, menu_area *source, uint8_t mod)
{
	if(!source) return NULL;
	if(!target) {
		return SetMenu(source, 0, 0, 0, 0, ENABLE, NULL);
	}
	target = FindMeunListHeard(target);
	menu_area *p = FindMeunListTail(target); 
	
	if(mod == 0)  { //超出屏幕，立即
		if(p->x+p->width*2 > SCREENWIDTH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width,target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x+p->width, p->y, p->width,p->high, p->checked, p);
		}
	}
	else if(mod == 1){
		if(p->x+p->width >= SCREENWIDTH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width, target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x+p->width, p->y, p->width, p->high, p->checked, p);
		}
	}
	else {
		return SetMenu(source, p->x+p->width, p->y, p->width, p->high, p->checked, p);
	}
}

/*
	功能：批量初始化
	target：所要附属于列表的任一菜单指针
	source：menu_area类型的数组
	n：数组个数
	mod：类型 0:有超出部分立即按照头创建 1:只有完全在屏幕下方才按头创建 2:仅在最后下方仿造
	menuinterface：统一链接的函数
	@ret：NULL

	注意：当 target==source 时， 两都会赋予menuinterface，不等时，仅source中会被赋予
*/

void BatchFastSimilarMenuDown(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target))
{
	uint16_t i = 0;
	
	if(target == NULL) return;
	
	if(target==source && n!=0) {
		source = &source[1];
		n-=1;
		target->menuinterface = menuinterface;
	}
	
	if(n>=1) {
		FastSimilarMenuDown(target, &source[0], mod)->menuinterface=menuinterface;
		for(i=1; i<n; i++) {
			FastSimilarMenuDown(&source[i-1], &source[i], mod)->menuinterface=menuinterface;
		}
	}
}
/*
	功能：批量初始化
	target：所要附属于列表的任一菜单指针
	source：menu_area类型的数组
	n：数组个数
	mod：类型 0:有超出部分立即按照头创建 1:只有完全在屏幕右侧才按头创建 2:仅在最后右侧仿造
	menuinterface：统一链接的函数

	注意：当 target==source 时， 两都会赋予menuinterface，不等时，仅source中会被赋予
*/

void BatchFastSimilarMenuRight(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target))
{
	uint16_t i = 0;
	
	if(target == NULL) return;
	
	if(target==source && n!=0) {
		source = &source[1];
		n-=1;
		target->menuinterface = menuinterface;
	}
	
	if(n>=1) {
		FastSimilarMenuRight(target, &source[0], mod)->menuinterface=menuinterface;
		for(i=1; i<n; i++) {
			FastSimilarMenuRight(&source[i-1], &source[i], mod)->menuinterface=menuinterface;
		}
	}
}
/**
 *  功能：链接到父类
 *  target：目标地址
 *  source：源地址 
 * 
 **/
void LinkToParentClass(menu_area *father, menu_area *sub)
{
	if(father==NULL || sub==NULL) return; //检查地址是否有效
	father->subclass = sub;
	sub->father = father;
}

/**
 * 功能：对菜单向上下偏移寻址
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向下偏移 1向上偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuListAddressing(menu_area *target, bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //检查地址是否有效
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->next == NULL){  //检查下一个是否存在
				return NULL;
			}
			p = p->next;
		}
		else
		{
			if(p->previous == NULL){ //检查上一个是否存在
				return NULL;
			}
			p = p->previous;
		}
	}
	return p;
}

/**
 * 功能：对菜单头的前后偏移寻址 
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向后偏移 1向前偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuHeadAddressing(menu_area *target,  bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //检查地址是否有效
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->subclass == NULL){ //检查子类是否存在
				return NULL;
			}
			p = p->subclass;
		}
		else
		{
			if(p->father == NULL){ //检查父类是否存在
				return NULL;
			}
			p = p->father;
		}
	}
	return p;
}

/*
	功能：寻找以目标单元为基准的第num个菜单可选中的菜单，无则返回原地址
		  输入空地址则返回空
	target: 基地址
	num：正值向下寻找，负值向上寻找
	返回值：寻址后的地址
*/
menu_area *NextCancheMenuList(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; // 绝对值
	uint16_t i;

	if(p==NULL) return NULL; //检查地址是否有效
	for(i=0; i<j;)
	{
		p=MenuListAddressing(p, num<0, 1);
		if(p==NULL || p==target) return target;
		if(p->checked == ENABLE) i++;
	}
	
	return p;
}


/*
	功能：寻找以目标单元为基准的第num个菜单可选中的菜单头，无则返回原地址
	target: 基地址
	num：正值向右寻找，负值向左寻找
	返回值：寻址后的地址
*/
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; // 绝对值
	uint16_t i;

	if(p==NULL) return NULL;//检查地址是否有效
	for(i=0; i<j;)
	{
		p=MenuHeadAddressing(p, num<0, 1); 
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	return p;	
}


/*
	功能：找到菜单所在菜单列表的菜单头，空指针返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListHeard(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;//检查地址是否有效
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; // 菜单列表头id 
		heard = heard->previous;
	}
	return heard;
}


/*
	功能：找到菜单所在菜单列表的菜单尾，空指针返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListTail(menu_area *target) 
{
	menu_area *tail=target;
	
	if(target == NULL) return NULL; //检查地址是否有效
	
	for( ; ; )
	{
		if(tail->next == NULL) break; //检查下一个是否存在
		if(tail->next->id == MENUHEARDID) break; // 循环中，下一个是否为头
		tail = tail->next;
	}
	return tail;
}

/*
	功能：返回当前显示列表的头，即使它不能被选中
	target：列表的任一菜单指针
	返回值：target为空返回空，其余返回显示菜单头
*/
menu_area *MenuListShowHead(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->id == MENUHEARDID) return targetmenu;//上一个为头
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; //上一个为结尾
		targetmenu = targetmenu->previous;
	}
}

/*
	功能：返回当前显示列表的尾，即使它不能被选中
	target：列表的任一菜单指针
	返回值：target为空返回NULL，其余返回显示菜单尾
*/
menu_area *MenuListShowTail(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; //下一个为空
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;//到达菜单列表尾
		if(targetmenu->menulistend == ENABLE) return targetmenu;//显示菜单尾
		targetmenu = targetmenu->next;
	}
}

/*
	功能：找到为ID的菜单
	target：所在菜单列表的任一菜单指针
	id：想要找到的id
	mod：	0：无则返回原指针，1：无则返回NULL
	@ret：找到的菜单指针
*/
menu_area *FindMenuOfID(menu_area *target, uint16_t id, bool mod)
{
	menu_area *p = target;
	uint16_t i=0;
	
	if(target == NULL) return NULL;
	if(id < MENUHEARDID) return (mod?NULL:target);
	
	if(id <= target->id) {
		for(i=0; i<(target->id - id); i++) {
			p = p->previous;
		}
		return p;
	}
	else {
		for(i=0; i<(id - target->id); i++) {
			p = p->next;
			if(p==NULL || p->id==MENUHEARDID) return (mod?NULL:target);
		}
		return p;
	}
}


/*
	功能： 目标菜单首尾相连，地址为空跳过 
	target：所在菜单列表的任一菜单 
*/
void MakeMenuListRing(menu_area *target)
{
	menu_area *heard, *tail;
	
	if(target == NULL) return; //检查地址是否有效
	
	heard = FindMeunListHeard(target); //找标准头
	tail = FindMeunListTail(target); //找标准尾
	
	tail->next = heard; //头尾链接
	heard->previous = tail;
}


static menu_area *menuChange = NULL;
static enum MenuState menuChangemes = Menu_noaction;
static uint8_t menuChangeFlag = 0;

/*
	功能：执行手动改变的菜单指针，并向按键广播消息
*/
static void changeMenu(void)
{
	if(menuChangeFlag==1)
	{
		TargetMenuPointrt.TargetMenuP = menuChange;
		menuChange = NULL;
		cushMes(menuChangemes);
		menuChangemes = Menu_noaction;
		menuChangeFlag=2;
	}
}
/*
	功能：手动指定要改变的菜单，并广播消息
	target：指定的菜单指针
	mes：消息
*/
void gotoMenu(menu_area *target, enum MenuState mes)
{
	if(menuChangeFlag==0)
	{
		menuChange = target;
		menuChangemes = mes;
		menuChangeFlag = 1;
	}
}






// ==================== 时 间 队 列 =================


/*
	功能：将菜单添加入时间列表，在该界面下，每ms执行指定菜单
	target：指定菜单
	menutime_obj: 已有的时间
	function：仅时间相关的宏
	ms：间隔时间
*/
void SetMenuTime(menu_area *target, menu_timems *menutime_obj, uint16_t function, uint16_t ms)
{
	if(target == NULL) return; // 检查地址是否有效
	target->menu_time = menutime_obj;
	menutime_obj->counttime=0; //默认起始计数值
	menutime_obj->timems = ms; //想要执行时间间隔
	target->specialfeatures |= function; //特殊功能注册
}

/*
	功能：将菜单添加入时间列表，在该界面下，每ms执行指定菜单
	target：指定菜单
	ms：间隔时间
*/
static void AddToMenuTimeList(menu_area *target, uint16_t ms)
{
	if(target == NULL) return; // 检查地址是否有效
	
	target->menu_time = (menu_timems *) MenuMalloc(sizeof(menu_timems));
	if(!(target->menu_time)) return;
	target->menu_time->counttime=0; //默认起始计数值
	target->menu_time->timems = ms; //想要执行时间间隔
}

// ====================初 始 化 列 表 ========================

static ListInit *MentListInitTarget = NULL; //菜单列表句柄头

/*
	功能：向列表加入初始化执行函数

	ListTarget：列表头句柄
	target：表句柄
	func：执行函数
*/
ListInit* AddToListInit(ListInit **ListTarget, ListInit *target, void *func)
{
	ListInit *p = *ListTarget;
	
	if(target==NULL) return NULL;
	
	if(*ListTarget == NULL)
		*ListTarget = target;
	else
	{
		while(p->next)
		{
			p = p->next;
		}
		p->next = target;
	}
	target->flag = 1;
	target->next = NULL;
	target->Target = func;
	
	return target;
}

/*
	功能：列表检查
	ListTarget：列表句柄

	ret：0 不能执行， 1 可以执行

	注意：if 末尾加  return！！
*/
uint8_t CheckListInit(ListInit *ListTarget)
{
	if(ListTarget->flag==1)
	{
		ListTarget->flag=0;
		return 1;
	}
	return 0;
}


// --- 菜单专用 ---

/*
	功能：开始菜单列表初始化
	ListTarget：列表句柄
*/
static void startMenuListInit(ListInit *ListTarget)
{
	ListInit *p = ListTarget;
	menu_area *tp;
	
	while(p)
	{
		if(p->flag)
		{
			tp = (menu_area *)(p->Target); //自定义 替换此
			if(tp->menuinterface)
				tp->menuinterface(tp);
		}
		p=p->next;
	}
}

/*
	功能：加入到菜单初始化列表中
	listp：列表句柄
	target：菜单句柄
*/
void AddToMenuListInit(menu_area *target)
{
	AddToListInit(&MentListInitTarget, &target->ListTarget, target);
}

/*
	功能：开始菜单列表初始化
*/
void sMenuListInit(void)
{
	startMenuListInit(MentListInitTarget);
}

/*
	功能：菜单列表检查

	target：菜单句柄

	ret：允许为1，否则为0

  注意：if 末尾加  return！！
*/
uint8_t CkeckMenuList(menu_area *target)
{
	return CheckListInit(&target->ListTarget);
}


// ====================== 特 殊 功 能 ========================


/*
	功能：特殊功能注册
	target：要注册的菜单
	function：特殊功能表里的宏，可或
	ms；延时时间，含MenuTime时，ms为延时，不含时，ms不被使用，一般填NULL
*/
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms)
{
	if(target == NULL) return;
	
	if(function & MenuTime) {
		AddToMenuTimeList(target, ms);//时间队列
	}
	target->specialfeatures |= function; //特殊功能注册
}

/*
	功能：特殊功能检查，触发返回1，否则返回0
	target：想要检查的菜单
	function：想要检查的特殊功能
*/
bool TriggerCheck(menu_area *target, enum SpecialInformation function)
{
	bool triggerflag = 0;
	
	if(!target) return 0;
	if( (target->specfeattrigflag)&function ){
		triggerflag = 1;
		target->specfeattrigflag &= ~function;
	}
	return triggerflag;
}
/*
	说明：隶属于某一菜单，且只有一个的菜单列表，向菜单头赋予指定属性
	功能：菜单列表始终执行函数
	target: 列表的任意指针
*/
static MenuListOverall *MenuOverallPointer = NULL;

MenuListOverall *MenuOverall(menu_area *target)
{
	MenuListOverall *MenuOverallP;
	MenuListOverall *Tar = NULL;
	menu_area *p;
	
	p = FindMeunListHeard(target);
	if(!p) return NULL; //未找到菜单头，跳过
	if(p->specialfeatures & MenuHaveOverall) return NULL; //已有跳过
	
	Tar = (MenuListOverall *)MenuMalloc(sizeof(MenuListOverall)); //申请空间
	if( !(Tar) ) return NULL; //空间申请失败
	p->specialfeatures |= MenuHaveOverall; //菜单头赋予属性
	
	if(MenuOverallPointer==NULL){ //创建
		MenuOverallPointer = Tar;
	}
	else{
		MenuOverallP=MenuOverallPointer;
		while(MenuOverallP->next){
			MenuOverallP = MenuOverallP->next;
		}
		MenuOverallP->next = Tar;
	}
	Tar->Affiliation = p;
	Tar->menuinterface = NULL;
	Tar->next = NULL;
	
	return Tar;
}







// ========================== 图 形 化 =======================

/*
	功能：在目标菜单的 >相对位置< 画点
	target：目标菜单
	x, y；相对位置
	w_b：1有色 0无色
*/
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b)
{
	int16_t px=0, py=0; // 屏幕相对位置
	
	if(target == NULL) return;
	
	px = target->x + x;
	py = target->y + y;
	
	if(x>=target->width || y>=target->high) return; // 判断是否超出菜单边界
	if(x<0 || y<0) return;
	
	write_point(px, py, w_b);
}

/*
	功能：画目标菜单的矩形，空指针不画
	target: 目标菜单
*/
static void DrawMenuRectangle(MenuTargetTypedef *MenuPointer)
{	
	menu_area *target = MenuPointer->TargetMenuP;
	static TanLevPIDTypedef y_pid = {0.5, 0.3, -0.3, 15}; //d正值抑制，负值不抑制
	static TanLevPIDTypedef x_pid = {0.5, 0.3, -0.3, 15};
	static TanLevPIDTypedef w_pid = {0.5, 0.3, -0.3, 15};
	static TanLevPIDTypedef h_pid = {0.5, 0.3, -0.3, 15};
	
	int16_t y, x;
	int16_t width, high;
	uint8_t R = MenuPointer->R;
	
	
	if(target->width==0 || target->high==0) return;
	
	if(MenuPointer->style == ENABLE) //使能动效
	{ 
		y = TandemLevel_PID(&y_pid, target->y, MenuPointer->y);
		x = TandemLevel_PID(&x_pid, target->x, MenuPointer->x);
		width = TandemLevel_PID(&w_pid, target->width, MenuPointer->width);
		high = TandemLevel_PID(&h_pid, target->high, MenuPointer->high);
		if(high < R*2) high = R*2;  //防止过小出现错误
		if(width < R*2) width = R*2;
	}
	else 
	{
		x = target->x;
		y = target->y;
		width = target->width;
		high = target->high;
	}
	MenuPointer->x = x;
	MenuPointer->y = y;
	MenuPointer->width = width;
	MenuPointer->high = high;
	MenuPointer->R = R;
	DrawfillRoundRect(MenuPointer->x, MenuPointer->y, MenuPointer->width, MenuPointer->high, MenuPointer->R);
}





// ======================== 系 统 调 用 ====================

MenuTargetTypedef TargetMenuPointrt=// 实时目标菜单
{
	.TargetMenuP=NULL, 
	.LastTargetMenuP=NULL, 
	.x=60, 
	.y=30, 
	.width=SCREENWIDTH/2, 
	.high=SCREENHIGH/2, 
	.R=3
};

//以下函数与上完全相同，为防止同时使用该函数，故设置专用函数

static menu_area *MenuListShowHeadForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->previous == NULL) return targetmenu; //上一个不存在
		if(targetmenu->id == MENUHEARDID) return targetmenu;//上一个为头
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; //上一个为结尾
		targetmenu = targetmenu->previous;
	}
}

static menu_area *MenuListShowTailForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; //下一个为空
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;//到达菜单列表尾
		if(targetmenu->menulistend == ENABLE) return targetmenu;//显示菜单尾
		targetmenu = targetmenu->next;
	}
}


static menu_area *FindMeunListHeardForHeart(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;//检查地址是否有效
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; // 菜单列表头id 
		heard = heard->previous;
	}
	return heard;
}


static menu_area *FindMeunListTailForHeart(menu_area *target) 
{
	menu_area *tail=target;
	
	if(target == NULL) return NULL; //检查地址是否有效
	
	for( ; ; )
	{
		if(tail->next == NULL) break; //检查下一个是否存在
		if(tail->next->id == MENUHEARDID) break; // 循环中，下一个是否为头
		tail = tail->next;
	}
	return tail;
}
/*
	功能：菜单心跳执行，每1ms执行该函数
*/
/*
	菜单心跳开始标志

	注意：请在菜单初始化的结尾使能该标志，防止中断与main同时调用相关函数
*/
bool MenuHeartTimeStart = DISABLE; //时间列表开始标志
static bool RefreshFlagForHeart = DISABLE; //时间列表刷新标志

static void MenuHeartTime(uint16_t ms)
{
	menu_area *p = TargetMenu; //菜单
	menu_area *MenuTail = NULL; //菜单尾
	menu_area *MenuShowHeard = NULL;
	menu_area *MenuShowTail = NULL;
	
	uint8_t flag = 0;
		
	if(p==NULL) return; //检查地址是否有效
	
	MenuShowHeard = MenuListShowHeadForHeart(p);// 找到开始显示的头
	MenuShowTail = MenuListShowTailForHeart(p);//显示菜单尾
	p = FindMeunListHeardForHeart(p);// 找到列表头
	MenuTail = FindMeunListTailForHeart(p);//找到列表尾
	
	for( ; ; )
	{
		if(p->menu_time != NULL){ //是否创建	
			if(p == MenuShowHeard) flag = 1;
			if(flag) {
				if( (p->specialfeatures)& MenuTimeForce ){
					p->menu_time->counttime+=ms; //计时
					if( (p->menu_time->counttime) >= p->menu_time->timems){ //到达计时点
						p->menu_time->counttime=0; //计时复位
						p->specfeattrigflag |= MenuTime; //赋值状态
						if(p->menuinterface){
							p->menuinterface(p); //执行指向函数
						}
						RefreshFlagForHeart = ENABLE;
					}
				}
				else {
					p->menu_time->counttime+=ms; //计时
					if( (p->menu_time->counttime) >= p->menu_time->timems){ //到达计时点
						p->menu_time->counttime=0; //计时复位
						p->specfeattrigflag |= MenuTime; //赋值状态
						RefreshFlagForHeart = ENABLE;
					}
				}
			}
			else {
				if( (p->specialfeatures) & MenuTimeForce ){
					p->menu_time->counttime+=ms; //计时
					if( (p->menu_time->counttime) >= p->menu_time->timems){ //到达计时点
						p->menu_time->counttime=0; //计时复位
						p->specfeattrigflag |= MenuTime; //赋值状态
						if(p->menuinterface){
							p->menuinterface(p); //执行指向函数
						}
						RefreshFlagForHeart = ENABLE;
					}
				}
			}
			if(p == MenuShowTail) flag = 0;
		}
		if(p==MenuTail) break;
		p=p->next;
	}
}

//菜单心跳时钟
// ms: 间隔几毫秒
void MenuTicker_ms(uint16_t ms)
{
	if(MenuHeartTimeStart==DISABLE) return; //是否开始
	
	MenuHeartTime(ms);
	FunctionTickerRunIRQ(ms);
	KeyDisposeISR(ms);
}


/*
	功能：菜单列表各个菜单内容循环显示
*/
static void MenuListInterface(menu_area *target)
{
	menu_area *p = NULL;
	menu_area *pTail = NULL;
		
	p = MenuListShowHead(target); //显示头
	pTail = MenuListShowTail(target);//显示尾
	
	for( ; ; )
	{
		if( (inScreen(p) || p->specialfeatures&NotIgnore) && p->menuinterface){
			p->menuinterface(p);
		}
		MenuRest(); //菜单复位
		if(p == pTail) break; 
		p = p->next;
	}
}

/*
	功能：特殊功能运行
	target： 实时目标菜单指针
*/

static void SpecialFunctionRun(menu_area *target)
{
	static menu_area *lastmenuheard = NULL; 
	menu_area *lastmenutail = FindMeunListTail(lastmenuheard); 
	static menu_area *lastshowmenuheard = NULL;
	menu_area *lastshowmenutail = MenuListShowTail(lastshowmenuheard);	
	
	menu_area *nowmenuheard = FindMeunListHeard(target); 
	menu_area *nowmenutail = FindMeunListTail(target);
	menu_area *nowshowmenuheard = MenuListShowHead(target);
	menu_area *nowshowmenutail = MenuListShowTail(target);
	
	menu_area *p = NULL;
	
	uint8_t flag = 0;
	
	if(!lastmenuheard){
		lastmenuheard = nowmenuheard;
		lastmenutail = FindMeunListTail(lastmenuheard); 
		lastshowmenuheard = nowshowmenuheard;
		lastshowmenutail = MenuListShowTail(lastshowmenuheard);	
	}
	
	if( lastmenuheard != nowmenuheard){ //切换菜单级
		for( p=lastmenuheard; ; ){ //上一级退出执行
			if(p->specialfeatures & ExitMenu) {
				p->specfeattrigflag |= ExitMenu;
				p->menuinterface(p);
			}
			if(p==lastshowmenuheard){
				flag = 1;
			}
			if(flag) {
				if(p->specialfeatures & ExitShowMenuList) {
					p->specfeattrigflag |= ExitShowMenuList;
					p->menuinterface(p);
				}
			}
			if(p == lastshowmenutail) {
				flag = 0;
			}
			if(p == lastmenutail) break;
			p = p->next;
		}
		for(p=nowmenuheard; ; ){//本级进入执行
			if(p->specialfeatures & EnterMenu) {
				p->specfeattrigflag |= EnterMenu;
				p->menuinterface(p);
			}
			if(p==nowshowmenuheard){
				flag = 1;
			}
			if(flag) {
				if(p->specialfeatures & EnterShowMenuList) {
					p->specfeattrigflag |= EnterShowMenuList;
					p->menuinterface(p);
				}
			}
			if(p == nowshowmenutail) {
				flag = 0;
			}
			if(p == nowmenutail) break;
			p = p->next;
		}
		lastmenuheard = nowmenuheard;
		lastshowmenuheard = nowshowmenuheard;
		ClearnBuff(); // 清空缓存
		return;
	}
	
	if(lastshowmenuheard != nowshowmenuheard){ //切换显示菜单
		for(p=lastshowmenuheard; ; ) { //上一显示列表退出执行
			if(p->specialfeatures & ExitShowMenuList) {
				p->specfeattrigflag |= ExitShowMenuList;
				p->menuinterface(p);
			}
			if(p == lastshowmenutail) break;
			p = p->next;
		}
		for(p=nowshowmenuheard; ; ) { //本显示列表进入执行
			if(p->specialfeatures & EnterShowMenuList) {
				p->specfeattrigflag |= EnterShowMenuList;
				p->menuinterface(p);
			}
			if(p == nowshowmenutail) break;
			p = p->next;
		}
		lastshowmenuheard = nowshowmenuheard;
		ClearnBuff(); // 清空缓存
	}
}

//菜单全局显示
static void MenuListOverallRun(menu_area *target)
{
	menu_area *p;
	MenuListOverall *MenuListOverallP=MenuOverallPointer;
	
	if(!MenuListOverallP) return; //是否有全局
	p = FindMeunListHeard(target);
	if( !(p->specialfeatures & MenuHaveOverall) ) return; //是否创建
	
	while(MenuListOverallP->Affiliation != p){ //找到对应列表头的全局
		if( !(MenuListOverallP->next) ) return;
		MenuListOverallP = MenuListOverallP->next;
	}
	if(MenuListOverallP->menuinterface){
		MenuListOverallP->menuinterface();
	}
}

/*
	功能：设备输入状态改变实时目标菜单指针
*/
static void StateToPointer(void)
{
	#if !MENU_FATHER
		menu_area *p = NULL;
	#endif

	if(TargetMenuPointrt.LastTargetMenuP != TargetMenuPointrt.TargetMenuP)
		menuChangeFlag=0;
	
	TargetMenuPointrt.LastTargetMenuP = TargetMenuPointrt.TargetMenuP;
	
	if(ResponseEnable == DISABLE) 
	{
		StopKeyReceive(&ResponseEnable);
		return;
	}

	MesSIF = StatusInformation; //按键状态记录 到 消息
	
	switch(StatusInformation)
	{
		case Menu_up: 		TargetMenuPointrt.TargetMenuP=NextCancheMenuList(TargetMenuPointrt.TargetMenuP, -1);break;
		case Menu_down: 	TargetMenuPointrt.TargetMenuP=NextCancheMenuList(TargetMenuPointrt.TargetMenuP,  1);break;
		case Menu_Sub:  
			if(TargetMenuPointrt.TargetMenuP->subclass != NULL && TargetMenuPointrt.TargetMenuP->subclass->checked==ENABLE) {
				StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
		  	TargetMenuPointrt.TargetMenuP = TargetMenuPointrt.TargetMenuP->subclass;
			}
			break;
		case Menu_Father:  
			// 0: 返回至菜单头的父类   1: 返回至当前菜单的父类
			#if MENU_FATHER
				if(TargetMenuPointrt.TargetMenuP->father != NULL && TargetMenuPointrt.TargetMenuP->father->checked==ENABLE) {
					StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
					TargetMenuPointrt.TargetMenuP = TargetMenu->father;
				}
			#else
				p = FindMeunListHeard(TargetMenuPointrt.TargetMenuP);
				if(p->father != NULL && p->father->checked==ENABLE) {
					StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
					TargetMenuPointrt.TargetMenuP = p->father;
				}
			#endif
			break;
		default:break;
	}
}

// 单个菜单刷新后复位
static void MenuRest(void)
{
	RestFont(); //恢复默认字体
}

//列表菜单刷新后复位
static void endRest(void)
{

}

// 输入禁止时
static void StopKeyReceive(uint8_t *p)
{
	MesSIF = Menu_noaction; //防止禁止时错误的按键状态被广播
	
	if(p == &InuptEnable) //禁止按键输入
	{
		StatusInformationAlways = Menu_noaction;
		StatusInformation = Menu_noaction;
	}
	else if(p == &ResponseEnable) //按键不响应
	{
		
	}
}

// 运行
static void MenuAlwaysRun_PVH(void)
{
	changeMenu(); //手动改变菜单
	
	if(TargetMenuPointrt.LastTargetMenuP != TargetMenu) //按键消息广播
	{
		cushMes(MesSIF); //向缓冲区写消息
	}
}
static void MenuAlwaysRun_PLH(void) // 
{
	cushMesBro(Message_Key); //广播缓冲区的消息
}




/*
	功能：菜单选中风格
*/
static void MenuCheckedStyle(MenuTargetTypedef *MenuPointer)
{
	if(MenuPointer->show == ENABLE) 
	{
		GRAPHICSSHOWMANNER = GraphicsRollColor;
		DrawMenuRectangle(MenuPointer); // 用户菜单选中
		GRAPHICSSHOWMANNER = GraphicsNormal;
	}
}

//菜单系统初始化
void MenuSysBaseInit(void) 
{
	// 指示器初始化
	TargetMenuPointrt.style = DISABLE; //不使能指针动效
	TargetMenuPointrt.show = ENABLE;  //指针显示

	// OLED
	OLED_Init(); // OLED初始化
	
	// 输入设备
	KeyDisInit(&MenuKey, 1); // 按键长按初始化
	
}

/*
	功能：菜单运行函数。 刷新屏幕 之前，越靠后屏幕显示优先级越高
*/
void MenuRun(void)
{
	EquipmentState(); //输入设备
	FunctionTickerRun();
	
	if(RefreshFlagForHeart == ENABLE){ //时间列表刷新标志
		RefreshFlagForHeart = DISABLE;
		MenuRefresh(0);
	}
	
	if((ScreenPara.refresh) && TargetMenu)
	{
		StateToPointer(); // 设备输入状态改变实时目标菜单指针
		
		ClearnBuff(); // 清空缓存
		MenuAlwaysRun_PVH();
		MenuAlwaysRun_PH();
		MenuAlwaysRun_PLH();
		SpecialFunctionRun(TargetMenu); //特殊功能运行
		MenuAlwaysRun_PM();
		MenuListOverallRun(TargetMenu); //菜单全局
		MenuListInterface(TargetMenu); //依次显示当前菜单列表
		MenuCheckedStyle(&TargetMenuPointrt);//菜单选中风格
		MenuAlwaysRun_PL();
		endRest();//系统复位
		disp_flush();// 刷新屏幕
		
		StatusInformation = Menu_noaction; //输入设备状态复位
	  ScreenPara.refresh--;// 刷新标志复位
	}
	AlwaysRun();
}








