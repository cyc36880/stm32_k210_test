#include "menu_tool.h"

// *******************************
// *********** �� Ϣ *************
// *******************************

MessageTypedef *Message_Key = NULL; //������Ϣ�㲥�б� 

static uint32_t cushMesData = 0;//������Ϣ
static bool couhIsNULL=true; //�����Ƿ�Ϊ�ձ�־

/**
	���ܣ�  ��Ϣ�㲥
	mes��   ��Ϣ������ 
	mesage��Ҫ�㲥����Ϣ 
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

//���ͻ���
//ע�⣺��Ϣ������Ϊ�ղ���д��ɹ�
void cushMes(uint32_t mesage)
{
	if(couhIsNULL == true)
	{
		cushMesData = mesage;
		couhIsNULL = false;
	}
}

/*
	���ܣ�������Ϣ���㲥����������Ϣ�����ͳɹ���״̬�Զ���λ
	mes����Ϣ�ġ�ͷ�����
	ע�⣺���Զ��жϣ������ظ�����
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
	���ܣ��жϻ������Ƿ�Ϊ��
	ret��1 �գ�0��Ϊ��
*/
uint8_t cushIsNull(void)
{
	return couhIsNULL;
}

// ���û���Ϊ�� true �� false
void SetCushNull(bool sta)
{
	couhIsNULL = sta;
}

// ��������
uint32_t readCush(void)
{
	return cushMesData;
}



/*
	���ܣ���Ϣ��ʼ��
	Target��Ҫ�������Ϣ�б�ͷ�����ָ�룩 
	mes��Ҫ��ʼ������Ϣ 
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



// ======================== �� �� �� �� �� ʾ ========================

/*
	���ܣ� �� �˵��б� ��x����+ֵ
	target����ǰ��������һ�˵�ָ��
	showSX���б�������ʾ����ʼy����
	showEX���б�������ʾ�Ľ���y����
	x_dat������ֵ
	style: ��ʾ���� 0 ������ʾ�����ڵĻᱻʹ��menulistend�����ˢ��Ч��

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
	���ܣ�������ʾ
	target����ǰ��������һ�˵�ָ�룬 >> �����ο�Ϊ�� <<
	showSY���б�������ʾ����ʼy���� ͷ����
	showEY���б�������ʾ�Ľ���y���� ������
	TarSY�� ָ���������ʼy����     ͷ���� Ӧ >= showSY
	TarEY�� ָ������Ľ���y����     ������ Ӧ <= showEY
	style�� ��αƽ���ʾ.   0 ֱ��ˢ����Ŀ��   1 ������ˢ����Ŀ��

	ret: ��������������ɷ���1�����򷵻�0

	ע�⣺�ú�����ı�����˵���menulistend���ԣ��ڸò˵����ڵĲ˵��б��У����ڳ���˵����⹦�ܣ�
		����ʹ��EnterMenu��ExitMenu��EnterShowMenuList��ExitShowMenuList���ڶ�δ�������
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
		if(style == 0)  //ֱ��
			ChangeMenuX(target, showSX, showEX, TarSX-target->x, 0);
		else { //����
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

// ======================== �� �� �� �� �� ʾ ========================

/*
	���ܣ��� �˵��б� ��y����+ֵ
	target����ǰ��������һ�˵�ָ��
	showSY���б�������ʾ����ʼy����
	showEY���б�������ʾ�Ľ���y����
	y_dat������ֵ
	style: ��ʾ���� 0 ������ʾ�����ڵĻᱻʹ��menulistend�����ˢ��Ч��

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
	���ܣ�������ʾ
	target����ǰ��������һ�˵�ָ��, >> �����ο�Ϊ�� <<
	showSY���б�������ʾ����ʼy���� ͷ����
	showEY���б�������ʾ�Ľ���y���� ������
	TarSY�� ָ���������ʼy����     ͷ���� Ӧ >= showSY
	TarEY�� ָ������Ľ���y����     ������ Ӧ <= showEY
	style�� ��αƽ���ʾ   0 ֱ��ˢ����Ŀ��   1 ������ˢ����Ŀ��

	ret: ��������������ɷ���1�����򷵻�0

	ע�⣺�ú�����ı�����˵���menulistend���ԣ��ڸò˵����ڵĲ˵��б��У����ڳ���˵����⹦�ܣ�
		����ʹ��EnterMenu��ExitMenu��EnterShowMenuList��ExitShowMenuList���ڶ�δ�������
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
		if(style == 0)  //ֱ��
			ChangeMenuY(target, showSY, showEY, TarSY-target->y, 0);
		else { //����			
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


// ************* �� �� �� �� ****************

/*
	���ܣ��˵��ѵ������£�
	target�����
	mod: 0:��ͷ�˵��ص�  1���뵱ǰ�˵��ص�
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
	���ܣ�ʹ�ѵ��Ĳ˵� ��ͷ�˵� ΪĿ��ָ�չ��״̬
	target�����
	mod�� 0 ����չ��  1������չ��
	style: 0 ֱ��չ��   1 ����չ��
	ret : ��ȫչ��������չ������1�����򷵻�0
	

	ע�⣺����չ���󣬿�����ԭ�е����겻һ��,��ʹ�������
				������ʹ�˵��������������޹����˵��б�Ҫʹ�ô˺���
 */
uint8_t MenuCoorRecovery(menu_area *target, uint8_t mod, uint8_t style)
{
	uint8_t state=1;
	static TanLevPIDTypedef pid = {0.4, 0, 0, 15};//pid����
	
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
		if(mod == 0) // ����չ��
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
		else         // ����չ��
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
	���ܣ��б��л�ʵ�����ж���Ч����
				�б�� չ�� �� ����
	mes����Ϣ���--��������Ϣ��
	start��ָʾ��������ʼλ��
	end��  ָʾ���������λ��
	dir��  ����0 ����1����
	
	ע�⣺��ʹ�á����⹦�ܡ���EnterMenu���ѵ��˵� ��MenuCoorAlignment�� ʱ���˺���Ӧ��
			MenuCoorAlignmentִ�е����ȼ��ͣ�����ɷ����� MenuAlwaysRun_PM��
*/
void ListSwitchIns(MessageTypedef *key_mes, bool dir, int16_t start, int16_t end)
{
	uint8_t sta=0;
	uint8_t (*ScrollingDisplay)(menu_area *target, int16_t showSY, int16_t showEY, int16_t TarSY, int16_t TarEY, uint8_t style);

	if(dir == 0)
		ScrollingDisplay = ScrollingDisplay_Y;
	else
		ScrollingDisplay = ScrollingDisplay_X;

	if( MesHave((*key_mes)) ) //����Ϣ
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
// ************ �� �� �� ʱ ִ �� ************
// *******************************************

static FunctionTicker *FunctionTickerPointer = NULL;
static uint8_t FTrigFlag=DISABLE;

/*
	���ܣ�������ʱִ��
	FTtarget�����
	ms����ʱʱ��
	RunMod��ģʽ  normalRun��interruptRun
	Function�� ִ�к���ָ��
	parpass����Ҫ���ݵĲ���
*/
FunctionTicker *SetFunctionTicker(FunctionTicker *FTtarget, uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(uint32_t parpass), uint32_t parpass)
{
	if(FTtarget == NULL) return NULL;
	
	if(FunctionTickerPointer == NULL) { //ͷָ���Ƿ�Ϊ��
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
	���ܣ�������ʱִ��
	ms����ʱʱ��
	RunMod��ģʽ  normalRun��interruptRun
	Function�� ִ�к���ָ��
	parpass����Ҫ���ݵĲ���
*/
FunctionTicker *AddToFunctionTicker(uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(uint32_t parpass), uint32_t parpass)
{
	FunctionTicker *FTtarget;
	
	FTtarget = MenuMalloc(sizeof(FunctionTicker));
	
	if(FTtarget == NULL) return NULL;
	
	return SetFunctionTicker(FTtarget, ms, RunMod, Function, parpass);
}

/*
	���ܣ����ж��в�ѯ�����Ƿ��ִ��
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
	���ܣ���while�в�ѯ�����Ƿ�ִ��
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
// *************** ���� *****************
// **************************************


static KeyTypedef *KeyListHeart = NULL; //����ͷ�ڵ�

// ������ʼ��
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
	key->keypt = 300; //������Ӧ�ٶȣ������·�һ���޸ģ�
	key->keynpflag=0; // key n put flag
	key->keypsc=0;// key puts count
	key->keylpt=0;// key long put time
	key->keystate=0;
	key->keycr=0; //key can read
	key->next=NULL;
}


// ��ȡ����״̬
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



// �������� �ж�
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
			
			if(p->keypt > 0) { // n��
				if(p->keynpflag==0) {
					p->keypt = 200;
					p->keypsc++;
					p->keynpflag=1;
				}
				p->keystate=1; 
			}
			
			if(p->keypt==0) { // ����
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
// ****************** �������� ******************
// **********************************************

// t=0: dat�������һ��  t=1��dat2�ı�����һ��!!!ֻ�ܱ�����һ��!!!
bool tRunOne(TypeRunOne *RunOne, bool t, uint32_t dat1,uint32_t dat2)
{
	if(t==0) //0: ֵΪdat����һ��
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
	else //1��dat�ı�����һ��
	{
		if(RunOne->dat != dat2)
		{
			RunOne->dat = dat2;
			return 1;
		}
	}
	return 0;
}


/******************** ����PID **********************/

/*
	���ܣ�pid��ʼ��
	pidhandle�����
	maxval�����շ���ֵ�ľ���ֵ��ֵ
	P��I��D��pid����
*/
TanLevPIDTypedef *TanLevPIDInit(TanLevPIDTypedef *pidhandle, uint16_t maxval, float P, float I, float D)
{
	ClearnMemory(pidhandle, sizeof(TanLevPIDTypedef)); //�ڴ����
	
	pidhandle->P = P;
	pidhandle->I = I;
	pidhandle->D = D;
	pidhandle->maxval = maxval;
	
	return pidhandle;
}

/*
	���ܣ����pid����ֵ

	wantval��������ֵ
	nowval��Ŀǰֵ
*/
float TandemLevel_PID(TanLevPIDTypedef *pidhandle, float wantval, float nowval)
{
	float retval=0;
	float diffval = wantval - nowval; //��ֵ
	float d_val=0;
	
	retval = diffval*pidhandle->P; //pֵ
	retval = retval>pidhandle->maxval?pidhandle->maxval:\
					(retval<-pidhandle->maxval?-pidhandle->maxval:retval);

	pidhandle->i_val += diffval; //����
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





// ****** ���� **********

/*
	���ܣ��ڴ�����
	
	
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








