#include "menu_tool.h"

// *******************************
// *********** 消 息 *************
// *******************************

MessageTypedef *Message_Key = NULL; //按键消息广播列表 

static uint32_t cushMesData = 0;//缓冲消息
static bool couhIsNULL=true; //缓冲是否为空标志

/**
	功能：  消息广播
	mes：   消息链表句柄 
	mesage：要广播的消息 
 **/
void MessageBroadcast(MessageTypedef *mes, uint32_t mesage)
{
	if(mes == NULL) return;
	for( ; ; ) 
	{
		if(  !(mes->state & Mes_Have) ) {
			mes->message = mesage;
			mes->state |=  Mes_Have;
		}
		mes = mes->next;
		if(mes == NULL) break;
	}
}

//发送缓冲
//注意：消息缓冲区为空才能写入成功
void cushMes(uint32_t mesage)
{
	if(couhIsNULL == true)
	{
		cushMesData = mesage;
		couhIsNULL = false;
	}
}

/*
	功能：若有消息，广播缓冲区的消息，发送成功后，状态自动复位
	mes：消息的《头句柄》
	注意：已自动判断，不会重复发送
*/
void cushMesBro(MessageTypedef *mes)
{
	if(mes == NULL) return;
	
	if(couhIsNULL == false)
	{
		MessageBroadcast(mes, cushMesData);
		couhIsNULL = true;
	}
}

/*
	功能：判断缓冲区是否为空
	ret：1 空，0不为空
*/
uint8_t cushIsNull(void)
{
	return couhIsNULL;
}

// 设置缓冲为空 true 或 false
void SetCushNull(bool sta)
{
	couhIsNULL = sta;
}

// 读缓冲区
uint32_t readCush(void)
{
	return cushMesData;
}



/*
	功能：消息初始化
	Target：要列入的消息列表头句柄（指针） 
	mes：要初始化的消息 
*/

MessageTypedef * MessageInit(MessageTypedef** Target, MessageTypedef* mes)
{
	MessageTypedef *p = *Target;
	
	if(*Target == NULL) {
		*Target = mes;
	}
	else {
		for( ; ; )
		{
			if(p->next == NULL) break;
			p = p->next;
		}
		p->next = mes;
	}
	
	mes->message = 0;
	mes->state = 0;
	mes->next = NULL;
	
	return mes;
}



// ======================== 横 向 滚 动 显 示 ========================

/*
	功能： 对 菜单列表 的x坐标+值
	target：当前所处的仍一菜单指针
	showSX：列表允许显示的起始y坐标
	showEX：列表允许显示的结束y坐标
	x_dat：增加值
	style: 显示设置 0 不在显示区域内的会被使能menulistend，提高刷新效率

*/
static void ChangeMenuX(menu_area *target, int16_t showSX, int16_t showEX, int16_t x_dat, uint8_t style)
{
	menu_area *heard = NULL;
	menu_area *tail = NULL;

	if(!target) return;

	heard = FindMeunListHeard(target);
	tail = FindMeunListTail(target);

	for( ; ;) {
		heard->x += x_dat;
		if(style == 0) {
			if( heard->x < showSX  || (heard->x + heard->width) > showEX) {
				heard->menulistend = ENABLE;
			}
			else{
				heard->menulistend = DISABLE;
			}
		}
		else 
		{
			heard->menulistend = DISABLE;
		}
		
		if(heard == tail) break;
		heard = heard->next;
	}
}

/*
	功能：滚动显示
	target：当前所处的任一菜单指针， >> 滚动参考为此 <<
	showSY：列表允许显示的起始y坐标 头坐标
	showEY：列表允许显示的结束y坐标 底坐标
	TarSY： 指针允许的起始y坐标     头坐标 应 >= showSY
	TarEY： 指针允许的结束y坐标     底坐标 应 <= showEY
	style： 逐次逼近显示.   0 直接刷新至目标   1 逐像素刷新至目标

	ret: 无需滚动或滚动完成返回1，否则返回0

	注意：该函数会改变大量菜单的menulistend属性，在该菜单所在的菜单列表中，对于出入菜单特殊功能，
		建议使用EnterMenu，ExitMenu。EnterShowMenuList与ExitShowMenuList存在多次触发问题
*/
uint8_t ScrollingDisplay_X(menu_area *target, int16_t showSX, int16_t showEX, int16_t TarSX, int16_t TarEX, uint8_t style) 
{
	static TanLevPIDTypedef pid = {0.5, 0, 0, 35};
	uint8_t offsets=0;
	
	uint8_t state = 1;

	menu_area *p;

	if(pid.P < 0.5) offsets = 1/pid.P;
	else if(pid.P < 1) offsets=1;
	else offsets = 0;
	
	p = FindMeunListHeard(target);
	
	if( !(p->specialfeatures & MenuScrollingX) ) {
		p->specialfeatures |= MenuScrollingX;
		ChangeMenuX(target, showSX, showEX, 0, style);
	}
	
	if(showSX > TarSX) showSX = TarSX;
	if(TarEX > showEX) TarEX = showEX;
	
	if(target->x < TarSX) {
		if(style == 0)  //直达
			ChangeMenuX(target, showSX, showEX, TarSX-target->x, 0);
		else { //滚动
			ChangeMenuX(target, showSX, showEX, TarSX-TandemLevel_PID(&pid, target->x-offsets, TarSX), 1);
				state = 0;
		}
	}
	else if(target->x+target->width-1 > TarEX) {
		if(style == 0) {
			ChangeMenuX(target, showSX, showEX, TarEX-target->x - target->width+1, 0);
		}
		else {
			ChangeMenuX(target, showSX, showEX, TarEX-TandemLevel_PID(&pid, target->x+target->width+offsets, TarEX), 1);
			state = 0;
		}
	}
	return state;
}

// ======================== 纵 向 滚 动 显 示 ========================

/*
	功能：对 菜单列表 的y坐标+值
	target：当前所处的仍一菜单指针
	showSY：列表允许显示的起始y坐标
	showEY：列表允许显示的结束y坐标
	y_dat：增加值
	style: 显示设置 0 不在显示区域内的会被使能menulistend，提高刷新效率

*/
static void ChangeMenuY(menu_area *target, int16_t showSY, int16_t showEY, int16_t y_dat, uint8_t style)
{
	menu_area *heard = NULL;
	menu_area *tail = NULL;
	
	if(!target) return;
	
	heard = FindMeunListHeard(target);
	tail = FindMeunListTail(target);
	
	for( ; ;) {
		heard->y += y_dat;
		if(style == 0) {
			if( heard->y < showSY  || (heard->y + heard->high-1) > showEY ) {
				heard->menulistend = ENABLE;
			}
			else{
				heard->menulistend = DISABLE;
			}
		}
		else
		{
			heard->menulistend = DISABLE;
		}
		
		if(heard == tail) break;
		heard = heard->next;
	}
}

/*
	功能：滚动显示
	target：当前所处的任一菜单指针, >> 滚动参考为此 <<
	showSY：列表允许显示的起始y坐标 头坐标
	showEY：列表允许显示的结束y坐标 底坐标
	TarSY： 指针允许的起始y坐标     头坐标 应 >= showSY
	TarEY： 指针允许的结束y坐标     底坐标 应 <= showEY
	style： 逐次逼近显示   0 直接刷新至目标   1 逐像素刷新至目标

	ret: 无需滚动或滚动完成返回1，否则返回0

	注意：该函数会改变大量菜单的menulistend属性，在该菜单所在的菜单列表中，对于出入菜单特殊功能，
		建议使用EnterMenu，ExitMenu。EnterShowMenuList与ExitShowMenuList存在多次触发问题
*/
uint8_t ScrollingDisplay_Y(menu_area *target, int16_t showSY, int16_t showEY, int16_t TarSY, int16_t TarEY, uint8_t style) 
{
	static TanLevPIDTypedef pid = {0.5, 0, 0, 35};
	uint8_t offsets = 0;
	
	uint8_t state = 1;
	menu_area *p;
	
	if(target == NULL) return 1;
	
	if(pid.P < 0.5) offsets = 1/pid.P;
	else if(pid.P < 1) offsets=1;
	else offsets = 0;
	
	p = FindMeunListHeard(target);
	
	if( !(p->specialfeatures & MenuScrollingY) ) {
		p->specialfeatures |= MenuScrollingY;
		ChangeMenuY(target, showSY, showEY, 0, style);
	}
	
	if(showSY > TarSY) showSY = TarSY;
	if(TarEY > showEY) TarEY = showEY;
	
	
	if(target->y < TarSY) {
		if(style == 0)  //直达
			ChangeMenuY(target, showSY, showEY, TarSY-target->y, 0);
		else { //滚动			
			ChangeMenuY(target, showSY, showEY, TarSY-TandemLevel_PID(&pid, target->y-offsets, TarSY), 1);
		}
		state = 0;          
	}
	else if(target->y+target->high-1 > TarEY) {
		if(style == 0) 
			ChangeMenuY(target, showSY, showEY, TarEY-target->y - target->high+1, 0);
		else {
			ChangeMenuY(target, showSY, showEY, TarEY-TandemLevel_PID(&pid, target->y+target->high+offsets, TarEY), 1);
		}
		state = 0;
	}
	return state;
}


// ************* 菜 单 滑 动 ****************

/*
	功能：菜单堆叠（向下）
	target：句柄
	mod: 0:与头菜单重叠  1：与当前菜单重叠
*/
void MenuCoorAlignment(menu_area *target, uint8_t mod)
{
	if(target == NULL) return ;

	menu_area *MenuShowHead;
	menu_area *MenuShowTail = FindMeunListTail(target);
	menu_area *p;
	
	if(mod)
		MenuShowHead = target;
	else
		MenuShowHead = FindMeunListHeard(target);

	if(MenuShowHead == MenuShowTail) return ;

	p = MenuShowHead->next;

	for( ; ; )
	{
		p->x = MenuShowHead->x;
		p->y = MenuShowHead->y;

		if(p==MenuShowTail) break;
		p = p->next;
	}
}

/*
	功能：使堆叠的菜单 以头菜单 为目标恢复展开状态
	target：句柄
	mod： 0 纵向展开  1：横向展开
	style: 0 直接展开   1 动画展开
	ret : 完全展开或无需展开返回1，否则返回0
	

	注意：重新展开后，可能与原有的坐标不一致,会使其更紧凑
				！！会使菜单依次往后排序，无滚动菜单列表不要使用此函数
 */
uint8_t MenuCoorRecovery(menu_area *target, uint8_t mod, uint8_t style)
{
	uint8_t state=1;
	static TanLevPIDTypedef pid = {0.4, 0, 0, 15};//pid参数
	
	uint8_t offsets=0;
	
	if(target == NULL) return 1;

	menu_area *MenuShowHeard = FindMeunListHeard(target);
	menu_area *MenuShowTail = FindMeunListTail(target);
	menu_area *p;

	int16_t dif;

	if(MenuShowHeard == MenuShowTail) return 1;
	if(pid.P < 0.5) offsets = 1/pid.P+1;
	else if(pid.P < 1) offsets=1;
	else offsets = 0;
	
	p = MenuShowHeard->next;

	for( ; ; )
	{
		if(mod == 0) // 纵向展开
		{
			if(style==0) {
				p->y = p->previous->y + p->previous->high;
			}
			else 
			{
				dif = p->y - (p->previous->y + p->previous->high);
				if(dif != 0)
				{
					if(abs(dif)<3)
						p->y -= dif;
					else
						p->y = TandemLevel_PID(&pid, p->previous->y + p->previous->high+offsets, p->y);
					state = 0;
				}
			}
		}
		else         // 横向展开
		{
			if(style==0) {
				p->x = p->previous->x + p->previous->width;
			}
			else {
				dif = p->x - (p->previous->x + p->previous->width);
				if(dif != 0)
				{
					if(abs(dif)<3)
						p->x -= dif;
					else
						p->x = TandemLevel_PID(&pid, p->previous->x + p->previous->width+offsets, p->x);
					state = 0;
				}
			}
		}
		if(p==MenuShowTail) break;
		p = p->next;
	}
	
	return state;
}

/*
	功能：列表切换实例（有动画效果）
				列表的 展开 与 滚动
	mes：消息句柄--（按键消息）
	start：指示器允许起始位置
	end：  指示器允许结束位置
	dir：  方向。0 纵向，1横向
	
	注意：当使用“特殊功能”的EnterMenu来堆叠菜单 （MenuCoorAlignment） 时，此函数应比
			MenuCoorAlignment执行的优先级低，（如可放入在 MenuAlwaysRun_PM）
*/
void ListSwitchIns(MessageTypedef *key_mes, bool dir, int16_t start, int16_t end)
{
	uint8_t sta=0;
	uint8_t (*ScrollingDisplay)(menu_area *target, int16_t showSY, int16_t showEY, int16_t TarSY, int16_t TarEY, uint8_t style);

	if(dir == 0)
		ScrollingDisplay = ScrollingDisplay_Y;
	else
		ScrollingDisplay = ScrollingDisplay_X;

	if( MesHave((*key_mes)) ) //有消息
	{
		if(MesRead( (*key_mes) ) == Menu_Father) {
			if(MenuCoorRecovery(TargetMenu, dir, 1)) {
				if(ScrollingDisplay(TargetMenu, start, end, start, end, 1))
					MesClear( (*key_mes) );
			}
		}
		else {
			sta += MenuCoorRecovery(TargetMenu, dir, 1);
			sta += ScrollingDisplay(TargetMenu, start, end, start, end, 1);
			if(sta == 2) MesClear( (*key_mes) );
		}
	}
}


// *******************************************
// ************ 函 数 定 时 执 行 ************
// *******************************************

static FunctionTicker *FunctionTickerPointer = NULL;
static uint8_t FTrigFlag=DISABLE;

/*
	功能：函数定时执行
	FTtarget：句柄
	ms：定时时间
	RunMod：模式  normalRun或interruptRun
	Function： 执行函数指针
	parpass：需要传递的参数
*/
FunctionTicker *SetFunctionTicker(FunctionTicker *FTtarget, uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(uint32_t parpass), uint32_t parpass)
{
	if(FTtarget == NULL) return NULL;
	
	if(FunctionTickerPointer == NULL) { //头指针是否为空
		FunctionTickerPointer = FTtarget;
	}
	else {
		FunctionTicker *Tail = FunctionTickerPointer;
		
		while(Tail->next) {
			Tail=Tail->next;
		}
		Tail->next = FTtarget;
	}
	FTtarget->next = NULL;
	FTtarget->run = ENABLE;
	FTtarget->Flag = DISABLE;
	
	FTtarget->ms = ms;
	FTtarget->count = 0;
	FTtarget->RunMod = RunMod;
	FTtarget->NumOfRun = 0;
	FTtarget->Function = Function;
	FTtarget->parpass = parpass;
	return FTtarget;
}
/*
	功能：函数定时执行
	ms：定时时间
	RunMod：模式  normalRun或interruptRun
	Function： 执行函数指针
	parpass：需要传递的参数
*/
FunctionTicker *AddToFunctionTicker(uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(uint32_t parpass), uint32_t parpass)
{
	FunctionTicker *FTtarget;
	
	FTtarget = MenuMalloc(sizeof(FunctionTicker));
	
	if(FTtarget == NULL) return NULL;
	
	return SetFunctionTicker(FTtarget, ms, RunMod, Function, parpass);
}

/*
	功能：在中断中查询函数是否该执行
*/
void FunctionTickerRunIRQ(uint16_t ms)
{
	FunctionTicker *p = FunctionTickerPointer;
	
	FTrigFlag = ENABLE;
	while(p)
	{
		if(p->run == ENABLE) {
			p->count += ms;
			if(p->count >= p->ms) {
				p->count=0;
				
				if(p->NumOfRun > 1) p->NumOfRun--;
				else if(p->NumOfRun==1) p->run = DISABLE;
				
				if(p->RunMod == interruptRun) {
					if(p->Function) p->Function(p->parpass);
				}
				else if(p->RunMod == normalRun) {
					p->Flag = ENABLE;
				}
			}
		}
		p=p->next;
	}
}
/*
	功能：在while中查询函数是否执行
*/
void FunctionTickerRun(void)
{
	FunctionTicker *p = FunctionTickerPointer;
	
	if(FTrigFlag == ENABLE)
	{
		FTrigFlag = DISABLE;
		while(p) {
			if(p->Flag == ENABLE) {
				p->Flag = DISABLE;
				if(p->Function) p->Function(p->parpass);
			}
			p=p->next;
		}
	}
}

// **************************************
// *************** 按键 *****************
// **************************************


static KeyTypedef *KeyListHeart = NULL; //按键头节点

// 按键初始化
void KeyDisInit(KeyTypedef *key, uint8_t putsta)
{
	KeyTypedef *p = KeyListHeart;
	
	if(KeyListHeart==NULL) {
		KeyListHeart = key;
	}
	else {
		while(p->next != NULL) {
			p = p->next;
		}
		p->next = key;
	}
	key->KEYPUTSTA = putsta;
	key->ReadKey = !putsta;
	key->keypt = 300; //按键响应速度（需与下方一起修改）
	key->keynpflag=0; // key n put flag
	key->keypsc=0;// key puts count
	key->keylpt=0;// key long put time
	key->keystate=0;
	key->keycr=0; //key can read
	key->next=NULL;
}


// 获取按键状态
uint8_t GetKeyState(KeyTypedef *key, uint8_t *state, uint8_t *keycount)
{
	if(key->keycr == 1) {
		*state = key->keystate;
		*keycount = key->keypsc;
		key->keypsc = 0;
		key->keycr = 0;
		return 1;
	}
	else {
		return 0;
	}
}



// 按键处理 中断
void KeyDisposeISR(uint16_t ms)
{
	KeyTypedef *p = KeyListHeart;
	
	while(p) {
		if(p->keycr==1) goto loop;
		if(p->ReadKey == p->KEYPUTSTA) 
		{
			if(p->keypt > ms) 
				p->keypt -= ms;
			else 
				p->keypt = 0;
			
			if(p->keypt > 0) { // n击
				if(p->keynpflag==0) {
					p->keypt = 200;
					p->keypsc++;
					p->keynpflag=1;
				}
				p->keystate=1; 
			}
			
			if(p->keypt==0) { // 长按
				p->keylpt += ms;
				if(p->keylpt>200) {
					p->keylpt=0;
					p->keycr=1;
				}
				p->keystate=2; 
			}
		}
		else {
			p->keynpflag=0;
			p->keylpt=0;
			if(p->keypt<300) p->keypt+=ms;
			if( p->keystate==1 && ( p->keypt > 299 ) ) p->keycr=1;
		}
		loop:
		p = p->next;
	}
}







// **********************************************
// ****************** 单次运行 ******************
// **********************************************

// t=0: dat相等运行一次  t=1：dat2改变运行一次!!!只能被调用一次!!!
bool tRunOne(TypeRunOne *RunOne, bool t, uint32_t dat1,uint32_t dat2)
{
	if(t==0) //0: 值为dat运行一次
	{
		if(dat1 != dat2)
		{
			RunOne->floag = 1;
		}
		if(RunOne->floag==1 && (dat1 == dat2))
		{
			RunOne->floag = 0;
			return 1;
		}
	}
	else //1：dat改变运行一次
	{
		if(RunOne->dat != dat2)
		{
			RunOne->dat = dat2;
			return 1;
		}
	}
	return 0;
}


/******************** 串级PID **********************/

/*
	功能：pid初始化
	pidhandle：句柄
	maxval：最终返回值的绝对值最值
	P、I、D；pid参数
*/
TanLevPIDTypedef *TanLevPIDInit(TanLevPIDTypedef *pidhandle, uint16_t maxval, float P, float I, float D)
{
	ClearnMemory(pidhandle, sizeof(TanLevPIDTypedef)); //内存清空
	
	pidhandle->P = P;
	pidhandle->I = I;
	pidhandle->D = D;
	pidhandle->maxval = maxval;
	
	return pidhandle;
}

/*
	功能：输出pid最终值

	wantval：欲到达值
	nowval：目前值
*/
float TandemLevel_PID(TanLevPIDTypedef *pidhandle, float wantval, float nowval)
{
	float retval=0;
	float diffval = wantval - nowval; //差值
	float d_val=0;
	
	retval = diffval*pidhandle->P; //p值
	retval = retval>pidhandle->maxval?pidhandle->maxval:\
					(retval<-pidhandle->maxval?-pidhandle->maxval:retval);

	pidhandle->i_val += diffval; //积分
	pidhandle->i_val = pidhandle->i_val>pidhandle->maxval?pidhandle->maxval:\
									(pidhandle->i_val<-pidhandle->maxval?-pidhandle->maxval:pidhandle->i_val);
	
	d_val = diffval - pidhandle->last_diff;
	pidhandle->last_diff = diffval;
	d_val = d_val>pidhandle->maxval?pidhandle->maxval:\
					(d_val<-pidhandle->maxval?-pidhandle->maxval:d_val);
	

	retval = retval + pidhandle->i_val*pidhandle->I + d_val*pidhandle->D;
	retval = retval>pidhandle->maxval?pidhandle->maxval:\
					(retval<-pidhandle->maxval?-pidhandle->maxval:retval);
	
	return nowval + retval;
}





// ****** 工具 **********

/*
	功能：内存清零
	
	
*/
void ClearnMemory(void *m, uint16_t size)
{
	uint8_t *p = (uint8_t *) m;
	while(size--) {
		*p = 0;
		p++;
	}
}

uint32_t myabs(int32_t dat)
{
	return dat<0?-dat:dat;
}








