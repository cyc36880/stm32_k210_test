#include "menu.h"
#include "Graphicalfunctions.h"
#include "menu_tool.h"
#include "menufontshow.h"
/*
	0: ������������
	1: ����ص����ٴ���������Ӱ��StatusInformationAlways
*/
#define SCAN_MOD 1

// 0: �������˵�ͷ�ĸ���   1: ��������ǰ�˵��ĸ���
#define MENU_FATHER 0


void FunctionTickerRun(void); //��while�в�ѯ�����Ƿ�ִ��
void FunctionTickerRunIRQ(uint16_t ms); //���ж��в�ѯ�����Ƿ��ִ��
static void MenuRest(void); //�����˵�ˢ�º�ĸ�λ

// �����ֹʱ
static void StopKeyReceive(uint8_t *p);

static KeyTypedef MenuKey; //���� ����볤��
static enum MenuState MesSIF = Menu_noaction; //������Ϣ����¼����״̬

// ================================= �� �� ========================================

enum MenuState StatusInformation = Menu_noaction; //�����豸״̬
enum MenuState StatusInformationAlways = Menu_noaction; // �����豸״̬ <����ı�>


uint8_t InuptEnable = ENABLE; //������Ӧ
uint8_t ResponseEnable = ENABLE; //�˵��л���Ӧ

/*
	* ���ܣ������豸״̬
*/

static enum MenuState Scan(void)
{
	static enum MenuState adcstate = Menu_noaction;

	
	return adcstate;
}

/*
	����״̬��ȡ��򵥵Ĵ���
	�����е� return 1 ��ע��ˢ�£� ���޶�ʱˢ��ʱ��Ч����ʱˢ��������Ч��
*/

static bool KeyState(enum MenuState *k)
{
	uint8_t keystate=0;
	uint8_t keycount=0;
	
	static enum MenuState KeySt = Menu_noaction;
	// ͨ�ð�������
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
	// �������ӹ��ܴ��� -- ����볤��
	if(StatusInformationAlways >= Menu_up && StatusInformationAlways <= Menu_Father) 
		MenuKey.ReadKey = 1;
	else 
		MenuKey.ReadKey = 0;
	
	if(GetKeyState(&MenuKey, &keystate, &keycount ))
	{
		if(keystate == 1) { //��� keycountΪ�����Ĵ���
		}
		if(keystate == 2) { //����
			*k = StatusInformationAlways;
		}
		return 1;
	}
	
	return 0;
}


/*
	���ܣ������豸ɨ�裬�ú�������ϵͳ����
*/
static void EquipmentState(void)
{
	if(InuptEnable == DISABLE) { //�����Ƿ�ʹ��
		StopKeyReceive(&InuptEnable);
		return;
	}
	if(KeyState(&StatusInformation)){ //����ɨ��
		if(StatusInformation != Menu_noaction){ //�ǿ���
//			MenuRefresh(0); //��Ļˢ��
		}
	}
}


// ================================= �� Ļ ========================================

const unsigned int BUFFWEIGH = SCREENWIDTH;
const unsigned int BUFFHIGH  = DIVIDEUP(SCREENHIGH);
enum ScreenShowManner SCREENSHOWMANNER = ScreenNormal; //��Ļ��ʾ��ʽ
enum GraphicsShowManner GRAPHICSSHOWMANNER = GraphicsNormal; //ͼ����ʾ��ʽ

// ������Ļ��Ϣ
TypedefScreen ScreenPara = {BUFFWEIGH, BUFFHIGH, 1};

// ��ʾ����
unsigned char DisplayBuff[BUFFWEIGH * BUFFHIGH] = {0};


//�����ʾ����
void ClearnBuff(void)
{
	unsigned int i, j = BUFFWEIGH * BUFFHIGH;
	for(i=0; i<j; i++)
		DisplayBuff[i] = 0;
}
/*
	���ܣ���Ļ���ˢ��ע��
	mod��  0�������ڱ��е�ˢ�´��������򴴽�  ����������N��ˢ��
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
	���ܣ��жϲ˵��Ƿ�����Ļ��

	ret��0���� 1��
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



// ========================== �� �� ==================================

uint32_t MenuMallocSize = 0;//�˵�����Ŀռ��С
/*
	���ܣ��˵�����ռ�
	size������ռ���ֽ���
	@ret������ռ���׵�ַ��ʧ��Ϊ��
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
*	���ܣ������в˵�ע�����Ӳ˵� 
*	target�����еĲ˵�
*	x, y, width, high: Ϊע��˵���Ԫ��λ�����С 
*	checked: �ܷ�ѡ�У�1�� 0�� 
*	transfer��Ϊ NULL ע����ǲ˵�ͷ������˵���ַΪ��β���� 
*/
menu_area *SetMenu(menu_area *target, int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p = target;
	menu_area *k;
	
	if(!p) return NULL;
	
	ClearnMemory(target, sizeof(menu_area)); //�ռ����
	
	if(transfer != NULL)  //β������ 
	{
		k = FindMeunListTail(transfer); //�ҵ��ļ�β��
		k->next = p;
		p->previous = k;
		p->id = k->id + 1; 
	}
	else //�����˵�ͷ
	{
		p->id = MENUHEARDID; // �˵�ͷID 
		p->previous = NULL; //��һ��
	}

	p->x = x;
	p->y = y;
	p->width = width;
	p->high = high;
	p->checked =  checked; // �ܷ�ѡ�� 
	p->menulistend = DISABLE; //�˵��б����
	p->next = NULL;      //��һ��
	p->subclass = NULL;  //���� 
	p->father = NULL;    // ���� 
	p->menuinterface = NULL; // �˵�����
	p->specialfeatures = NULL; //���⹦��
	p->specfeattrigflag = NULL;//���⹦�ܴ�����־
	p->menu_time = NULL; //ʱ�����
	return p;
}

/*
*	���ܣ�ע�����Ӳ˵� 
*	x, y, width, high: Ϊע��˵���Ԫ��λ�����С 
*	checked: �ܷ�ѡ�У�1�� 0�� 
*	transfer��Ϊ NULL ע����ǲ˵�ͷ������˵���ַΪ��β���� 
*/
menu_area * AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p;
	
	p = (menu_area *) MenuMalloc(sizeof(menu_area)); //�����ڴ�ռ�
	
	return SetMenu(p, x, y, width, high, checked, transfer);
}

/*
	���ܣ�����Ŀ��˵��·�����
	target��Ҫ���Ƶ�Ŀ��˵�
	source��ӵ�еĲ˵�
	mod������ 0:�г���������������ͷ���� 1:ֻ����ȫ����Ļ�·��Ű�ͷ���� 2:��������·�����
*/

menu_area * FastSimilarMenuDown(menu_area *target, menu_area *source, uint8_t mod)
{
	if(!source) return NULL;
	if(!target) {
		return SetMenu(source, 0, 0, 0, 0, ENABLE, NULL);
	}
	target = FindMeunListHeard(target);
	menu_area *p = FindMeunListTail(target); 
	
	if(mod == 0)  { //������Ļ������
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
	���ܣ�����Ŀ��˵��Ҳ����
	target��Ҫ���Ƶ�Ŀ��˵�
	source��ӵ�еĲ˵�
	mod������ 0:�г���������������ͷ���� 1:ֻ����ȫ����Ļ�Ҳ�Ű�ͷ���� 2:��������Ҳ����
*/

menu_area * FastSimilarMenuRight(menu_area *target, menu_area *source, uint8_t mod)
{
	if(!source) return NULL;
	if(!target) {
		return SetMenu(source, 0, 0, 0, 0, ENABLE, NULL);
	}
	target = FindMeunListHeard(target);
	menu_area *p = FindMeunListTail(target); 
	
	if(mod == 0)  { //������Ļ������
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
	���ܣ�������ʼ��
	target����Ҫ�������б����һ�˵�ָ��
	source��menu_area���͵�����
	n���������
	mod������ 0:�г���������������ͷ���� 1:ֻ����ȫ����Ļ�·��Ű�ͷ���� 2:��������·�����
	menuinterface��ͳһ���ӵĺ���
	@ret��NULL

	ע�⣺�� target==source ʱ�� �����ḳ��menuinterface������ʱ����source�лᱻ����
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
	���ܣ�������ʼ��
	target����Ҫ�������б����һ�˵�ָ��
	source��menu_area���͵�����
	n���������
	mod������ 0:�г���������������ͷ���� 1:ֻ����ȫ����Ļ�Ҳ�Ű�ͷ���� 2:��������Ҳ����
	menuinterface��ͳһ���ӵĺ���

	ע�⣺�� target==source ʱ�� �����ḳ��menuinterface������ʱ����source�лᱻ����
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
 *  ���ܣ����ӵ�����
 *  target��Ŀ���ַ
 *  source��Դ��ַ 
 * 
 **/
void LinkToParentClass(menu_area *father, menu_area *sub)
{
	if(father==NULL || sub==NULL) return; //����ַ�Ƿ���Ч
	father->subclass = sub;
	sub->father = father;
}

/**
 * ���ܣ��Բ˵�������ƫ��Ѱַ
 * target��Ŀ��Ѱַ�˵� 
 * offset��ƫ�Ʋ��� 
 * upordown: 0����ƫ�� 1����ƫ�� 
 * ����ֵ�������Դ��ַƫ�ƺ�ĵ�ַ��offset=0����Դ��ַ�����򷵻�NULL 
 **/
menu_area * MenuListAddressing(menu_area *target, bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //����ַ�Ƿ���Ч
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->next == NULL){  //�����һ���Ƿ����
				return NULL;
			}
			p = p->next;
		}
		else
		{
			if(p->previous == NULL){ //�����һ���Ƿ����
				return NULL;
			}
			p = p->previous;
		}
	}
	return p;
}

/**
 * ���ܣ��Բ˵�ͷ��ǰ��ƫ��Ѱַ 
 * target��Ŀ��Ѱַ�˵� 
 * offset��ƫ�Ʋ��� 
 * upordown: 0���ƫ�� 1��ǰƫ�� 
 * ����ֵ�������Դ��ַƫ�ƺ�ĵ�ַ��offset=0����Դ��ַ�����򷵻�NULL 
 **/
menu_area * MenuHeadAddressing(menu_area *target,  bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //����ַ�Ƿ���Ч
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->subclass == NULL){ //��������Ƿ����
				return NULL;
			}
			p = p->subclass;
		}
		else
		{
			if(p->father == NULL){ //��鸸���Ƿ����
				return NULL;
			}
			p = p->father;
		}
	}
	return p;
}

/*
	���ܣ�Ѱ����Ŀ�굥ԪΪ��׼�ĵ�num���˵���ѡ�еĲ˵������򷵻�ԭ��ַ
		  ����յ�ַ�򷵻ؿ�
	target: ����ַ
	num����ֵ����Ѱ�ң���ֵ����Ѱ��
	����ֵ��Ѱַ��ĵ�ַ
*/
menu_area *NextCancheMenuList(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; // ����ֵ
	uint16_t i;

	if(p==NULL) return NULL; //����ַ�Ƿ���Ч
	for(i=0; i<j;)
	{
		p=MenuListAddressing(p, num<0, 1);
		if(p==NULL || p==target) return target;
		if(p->checked == ENABLE) i++;
	}
	
	return p;
}


/*
	���ܣ�Ѱ����Ŀ�굥ԪΪ��׼�ĵ�num���˵���ѡ�еĲ˵�ͷ�����򷵻�ԭ��ַ
	target: ����ַ
	num����ֵ����Ѱ�ң���ֵ����Ѱ��
	����ֵ��Ѱַ��ĵ�ַ
*/
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; // ����ֵ
	uint16_t i;

	if(p==NULL) return NULL;//����ַ�Ƿ���Ч
	for(i=0; i<j;)
	{
		p=MenuHeadAddressing(p, num<0, 1); 
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	return p;	
}


/*
	���ܣ��ҵ��˵����ڲ˵��б�Ĳ˵�ͷ����ָ�뷵��NULL 
	target���˵�ָ��
*/
menu_area *FindMeunListHeard(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;//����ַ�Ƿ���Ч
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; // �˵��б�ͷid 
		heard = heard->previous;
	}
	return heard;
}


/*
	���ܣ��ҵ��˵����ڲ˵��б�Ĳ˵�β����ָ�뷵��NULL 
	target���˵�ָ��
*/
menu_area *FindMeunListTail(menu_area *target) 
{
	menu_area *tail=target;
	
	if(target == NULL) return NULL; //����ַ�Ƿ���Ч
	
	for( ; ; )
	{
		if(tail->next == NULL) break; //�����һ���Ƿ����
		if(tail->next->id == MENUHEARDID) break; // ѭ���У���һ���Ƿ�Ϊͷ
		tail = tail->next;
	}
	return tail;
}

/*
	���ܣ����ص�ǰ��ʾ�б��ͷ����ʹ�����ܱ�ѡ��
	target���б����һ�˵�ָ��
	����ֵ��targetΪ�շ��ؿգ����෵����ʾ�˵�ͷ
*/
menu_area *MenuListShowHead(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->id == MENUHEARDID) return targetmenu;//��һ��Ϊͷ
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; //��һ��Ϊ��β
		targetmenu = targetmenu->previous;
	}
}

/*
	���ܣ����ص�ǰ��ʾ�б��β����ʹ�����ܱ�ѡ��
	target���б����һ�˵�ָ��
	����ֵ��targetΪ�շ���NULL�����෵����ʾ�˵�β
*/
menu_area *MenuListShowTail(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; //��һ��Ϊ��
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;//����˵��б�β
		if(targetmenu->menulistend == ENABLE) return targetmenu;//��ʾ�˵�β
		targetmenu = targetmenu->next;
	}
}

/*
	���ܣ��ҵ�ΪID�Ĳ˵�
	target�����ڲ˵��б����һ�˵�ָ��
	id����Ҫ�ҵ���id
	mod��	0�����򷵻�ԭָ�룬1�����򷵻�NULL
	@ret���ҵ��Ĳ˵�ָ��
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
	���ܣ� Ŀ��˵���β��������ַΪ������ 
	target�����ڲ˵��б����һ�˵� 
*/
void MakeMenuListRing(menu_area *target)
{
	menu_area *heard, *tail;
	
	if(target == NULL) return; //����ַ�Ƿ���Ч
	
	heard = FindMeunListHeard(target); //�ұ�׼ͷ
	tail = FindMeunListTail(target); //�ұ�׼β
	
	tail->next = heard; //ͷβ����
	heard->previous = tail;
}


static menu_area *menuChange = NULL;
static enum MenuState menuChangemes = Menu_noaction;
static uint8_t menuChangeFlag = 0;

/*
	���ܣ�ִ���ֶ��ı�Ĳ˵�ָ�룬���򰴼��㲥��Ϣ
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
	���ܣ��ֶ�ָ��Ҫ�ı�Ĳ˵������㲥��Ϣ
	target��ָ���Ĳ˵�ָ��
	mes����Ϣ
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






// ==================== ʱ �� �� �� =================


/*
	���ܣ����˵������ʱ���б��ڸý����£�ÿmsִ��ָ���˵�
	target��ָ���˵�
	menutime_obj: ���е�ʱ��
	function����ʱ����صĺ�
	ms�����ʱ��
*/
void SetMenuTime(menu_area *target, menu_timems *menutime_obj, uint16_t function, uint16_t ms)
{
	if(target == NULL) return; // ����ַ�Ƿ���Ч
	target->menu_time = menutime_obj;
	menutime_obj->counttime=0; //Ĭ����ʼ����ֵ
	menutime_obj->timems = ms; //��Ҫִ��ʱ����
	target->specialfeatures |= function; //���⹦��ע��
}

/*
	���ܣ����˵������ʱ���б��ڸý����£�ÿmsִ��ָ���˵�
	target��ָ���˵�
	ms�����ʱ��
*/
static void AddToMenuTimeList(menu_area *target, uint16_t ms)
{
	if(target == NULL) return; // ����ַ�Ƿ���Ч
	
	target->menu_time = (menu_timems *) MenuMalloc(sizeof(menu_timems));
	if(!(target->menu_time)) return;
	target->menu_time->counttime=0; //Ĭ����ʼ����ֵ
	target->menu_time->timems = ms; //��Ҫִ��ʱ����
}

// ====================�� ʼ �� �� �� ========================

static ListInit *MentListInitTarget = NULL; //�˵��б���ͷ

/*
	���ܣ����б�����ʼ��ִ�к���

	ListTarget���б�ͷ���
	target������
	func��ִ�к���
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
	���ܣ��б���
	ListTarget���б���

	ret��0 ����ִ�У� 1 ����ִ��

	ע�⣺if ĩβ��  return����
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


// --- �˵�ר�� ---

/*
	���ܣ���ʼ�˵��б��ʼ��
	ListTarget���б���
*/
static void startMenuListInit(ListInit *ListTarget)
{
	ListInit *p = ListTarget;
	menu_area *tp;
	
	while(p)
	{
		if(p->flag)
		{
			tp = (menu_area *)(p->Target); //�Զ��� �滻��
			if(tp->menuinterface)
				tp->menuinterface(tp);
		}
		p=p->next;
	}
}

/*
	���ܣ����뵽�˵���ʼ���б���
	listp���б���
	target���˵����
*/
void AddToMenuListInit(menu_area *target)
{
	AddToListInit(&MentListInitTarget, &target->ListTarget, target);
}

/*
	���ܣ���ʼ�˵��б��ʼ��
*/
void sMenuListInit(void)
{
	startMenuListInit(MentListInitTarget);
}

/*
	���ܣ��˵��б���

	target���˵����

	ret������Ϊ1������Ϊ0

  ע�⣺if ĩβ��  return����
*/
uint8_t CkeckMenuList(menu_area *target)
{
	return CheckListInit(&target->ListTarget);
}


// ====================== �� �� �� �� ========================


/*
	���ܣ����⹦��ע��
	target��Ҫע��Ĳ˵�
	function�����⹦�ܱ���ĺ꣬�ɻ�
	ms����ʱʱ�䣬��MenuTimeʱ��msΪ��ʱ������ʱ��ms����ʹ�ã�һ����NULL
*/
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms)
{
	if(target == NULL) return;
	
	if(function & MenuTime) {
		AddToMenuTimeList(target, ms);//ʱ�����
	}
	target->specialfeatures |= function; //���⹦��ע��
}

/*
	���ܣ����⹦�ܼ�飬��������1�����򷵻�0
	target����Ҫ���Ĳ˵�
	function����Ҫ�������⹦��
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
	˵����������ĳһ�˵�����ֻ��һ���Ĳ˵��б���˵�ͷ����ָ������
	���ܣ��˵��б�ʼ��ִ�к���
	target: �б������ָ��
*/
static MenuListOverall *MenuOverallPointer = NULL;

MenuListOverall *MenuOverall(menu_area *target)
{
	MenuListOverall *MenuOverallP;
	MenuListOverall *Tar = NULL;
	menu_area *p;
	
	p = FindMeunListHeard(target);
	if(!p) return NULL; //δ�ҵ��˵�ͷ������
	if(p->specialfeatures & MenuHaveOverall) return NULL; //��������
	
	Tar = (MenuListOverall *)MenuMalloc(sizeof(MenuListOverall)); //����ռ�
	if( !(Tar) ) return NULL; //�ռ�����ʧ��
	p->specialfeatures |= MenuHaveOverall; //�˵�ͷ��������
	
	if(MenuOverallPointer==NULL){ //����
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







// ========================== ͼ �� �� =======================

/*
	���ܣ���Ŀ��˵��� >���λ��< ����
	target��Ŀ��˵�
	x, y�����λ��
	w_b��1��ɫ 0��ɫ
*/
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b)
{
	int16_t px=0, py=0; // ��Ļ���λ��
	
	if(target == NULL) return;
	
	px = target->x + x;
	py = target->y + y;
	
	if(x>=target->width || y>=target->high) return; // �ж��Ƿ񳬳��˵��߽�
	if(x<0 || y<0) return;
	
	write_point(px, py, w_b);
}

/*
	���ܣ���Ŀ��˵��ľ��Σ���ָ�벻��
	target: Ŀ��˵�
*/
static void DrawMenuRectangle(MenuTargetTypedef *MenuPointer)
{	
	menu_area *target = MenuPointer->TargetMenuP;
	static TanLevPIDTypedef y_pid = {0.5, 0.3, -0.3, 15}; //d��ֵ���ƣ���ֵ������
	static TanLevPIDTypedef x_pid = {0.5, 0.3, -0.3, 15};
	static TanLevPIDTypedef w_pid = {0.5, 0.3, -0.3, 15};
	static TanLevPIDTypedef h_pid = {0.5, 0.3, -0.3, 15};
	
	int16_t y, x;
	int16_t width, high;
	uint8_t R = MenuPointer->R;
	
	
	if(target->width==0 || target->high==0) return;
	
	if(MenuPointer->style == ENABLE) //ʹ�ܶ�Ч
	{ 
		y = TandemLevel_PID(&y_pid, target->y, MenuPointer->y);
		x = TandemLevel_PID(&x_pid, target->x, MenuPointer->x);
		width = TandemLevel_PID(&w_pid, target->width, MenuPointer->width);
		high = TandemLevel_PID(&h_pid, target->high, MenuPointer->high);
		if(high < R*2) high = R*2;  //��ֹ��С���ִ���
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





// ======================== ϵ ͳ �� �� ====================

MenuTargetTypedef TargetMenuPointrt=// ʵʱĿ��˵�
{
	.TargetMenuP=NULL, 
	.LastTargetMenuP=NULL, 
	.x=60, 
	.y=30, 
	.width=SCREENWIDTH/2, 
	.high=SCREENHIGH/2, 
	.R=3
};

//���º���������ȫ��ͬ��Ϊ��ֹͬʱʹ�øú�����������ר�ú���

static menu_area *MenuListShowHeadForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->previous == NULL) return targetmenu; //��һ��������
		if(targetmenu->id == MENUHEARDID) return targetmenu;//��һ��Ϊͷ
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; //��һ��Ϊ��β
		targetmenu = targetmenu->previous;
	}
}

static menu_area *MenuListShowTailForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; //��һ��Ϊ��
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;//����˵��б�β
		if(targetmenu->menulistend == ENABLE) return targetmenu;//��ʾ�˵�β
		targetmenu = targetmenu->next;
	}
}


static menu_area *FindMeunListHeardForHeart(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;//����ַ�Ƿ���Ч
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; // �˵��б�ͷid 
		heard = heard->previous;
	}
	return heard;
}


static menu_area *FindMeunListTailForHeart(menu_area *target) 
{
	menu_area *tail=target;
	
	if(target == NULL) return NULL; //����ַ�Ƿ���Ч
	
	for( ; ; )
	{
		if(tail->next == NULL) break; //�����һ���Ƿ����
		if(tail->next->id == MENUHEARDID) break; // ѭ���У���һ���Ƿ�Ϊͷ
		tail = tail->next;
	}
	return tail;
}
/*
	���ܣ��˵�����ִ�У�ÿ1msִ�иú���
*/
/*
	�˵�������ʼ��־

	ע�⣺���ڲ˵���ʼ���Ľ�βʹ�ܸñ�־����ֹ�ж���mainͬʱ������غ���
*/
bool MenuHeartTimeStart = DISABLE; //ʱ���б�ʼ��־
static bool RefreshFlagForHeart = DISABLE; //ʱ���б�ˢ�±�־

static void MenuHeartTime(uint16_t ms)
{
	menu_area *p = TargetMenu; //�˵�
	menu_area *MenuTail = NULL; //�˵�β
	menu_area *MenuShowHeard = NULL;
	menu_area *MenuShowTail = NULL;
	
	uint8_t flag = 0;
		
	if(p==NULL) return; //����ַ�Ƿ���Ч
	
	MenuShowHeard = MenuListShowHeadForHeart(p);// �ҵ���ʼ��ʾ��ͷ
	MenuShowTail = MenuListShowTailForHeart(p);//��ʾ�˵�β
	p = FindMeunListHeardForHeart(p);// �ҵ��б�ͷ
	MenuTail = FindMeunListTailForHeart(p);//�ҵ��б�β
	
	for( ; ; )
	{
		if(p->menu_time != NULL){ //�Ƿ񴴽�	
			if(p == MenuShowHeard) flag = 1;
			if(flag) {
				if( (p->specialfeatures)& MenuTimeForce ){
					p->menu_time->counttime+=ms; //��ʱ
					if( (p->menu_time->counttime) >= p->menu_time->timems){ //�����ʱ��
						p->menu_time->counttime=0; //��ʱ��λ
						p->specfeattrigflag |= MenuTime; //��ֵ״̬
						if(p->menuinterface){
							p->menuinterface(p); //ִ��ָ����
						}
						RefreshFlagForHeart = ENABLE;
					}
				}
				else {
					p->menu_time->counttime+=ms; //��ʱ
					if( (p->menu_time->counttime) >= p->menu_time->timems){ //�����ʱ��
						p->menu_time->counttime=0; //��ʱ��λ
						p->specfeattrigflag |= MenuTime; //��ֵ״̬
						RefreshFlagForHeart = ENABLE;
					}
				}
			}
			else {
				if( (p->specialfeatures) & MenuTimeForce ){
					p->menu_time->counttime+=ms; //��ʱ
					if( (p->menu_time->counttime) >= p->menu_time->timems){ //�����ʱ��
						p->menu_time->counttime=0; //��ʱ��λ
						p->specfeattrigflag |= MenuTime; //��ֵ״̬
						if(p->menuinterface){
							p->menuinterface(p); //ִ��ָ����
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

//�˵�����ʱ��
// ms: ���������
void MenuTicker_ms(uint16_t ms)
{
	if(MenuHeartTimeStart==DISABLE) return; //�Ƿ�ʼ
	
	MenuHeartTime(ms);
	FunctionTickerRunIRQ(ms);
	KeyDisposeISR(ms);
}


/*
	���ܣ��˵��б�����˵�����ѭ����ʾ
*/
static void MenuListInterface(menu_area *target)
{
	menu_area *p = NULL;
	menu_area *pTail = NULL;
		
	p = MenuListShowHead(target); //��ʾͷ
	pTail = MenuListShowTail(target);//��ʾβ
	
	for( ; ; )
	{
		if( (inScreen(p) || p->specialfeatures&NotIgnore) && p->menuinterface){
			p->menuinterface(p);
		}
		MenuRest(); //�˵���λ
		if(p == pTail) break; 
		p = p->next;
	}
}

/*
	���ܣ����⹦������
	target�� ʵʱĿ��˵�ָ��
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
	
	if( lastmenuheard != nowmenuheard){ //�л��˵���
		for( p=lastmenuheard; ; ){ //��һ���˳�ִ��
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
		for(p=nowmenuheard; ; ){//��������ִ��
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
		ClearnBuff(); // ��ջ���
		return;
	}
	
	if(lastshowmenuheard != nowshowmenuheard){ //�л���ʾ�˵�
		for(p=lastshowmenuheard; ; ) { //��һ��ʾ�б��˳�ִ��
			if(p->specialfeatures & ExitShowMenuList) {
				p->specfeattrigflag |= ExitShowMenuList;
				p->menuinterface(p);
			}
			if(p == lastshowmenutail) break;
			p = p->next;
		}
		for(p=nowshowmenuheard; ; ) { //����ʾ�б����ִ��
			if(p->specialfeatures & EnterShowMenuList) {
				p->specfeattrigflag |= EnterShowMenuList;
				p->menuinterface(p);
			}
			if(p == nowshowmenutail) break;
			p = p->next;
		}
		lastshowmenuheard = nowshowmenuheard;
		ClearnBuff(); // ��ջ���
	}
}

//�˵�ȫ����ʾ
static void MenuListOverallRun(menu_area *target)
{
	menu_area *p;
	MenuListOverall *MenuListOverallP=MenuOverallPointer;
	
	if(!MenuListOverallP) return; //�Ƿ���ȫ��
	p = FindMeunListHeard(target);
	if( !(p->specialfeatures & MenuHaveOverall) ) return; //�Ƿ񴴽�
	
	while(MenuListOverallP->Affiliation != p){ //�ҵ���Ӧ�б�ͷ��ȫ��
		if( !(MenuListOverallP->next) ) return;
		MenuListOverallP = MenuListOverallP->next;
	}
	if(MenuListOverallP->menuinterface){
		MenuListOverallP->menuinterface();
	}
}

/*
	���ܣ��豸����״̬�ı�ʵʱĿ��˵�ָ��
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

	MesSIF = StatusInformation; //����״̬��¼ �� ��Ϣ
	
	switch(StatusInformation)
	{
		case Menu_up: 		TargetMenuPointrt.TargetMenuP=NextCancheMenuList(TargetMenuPointrt.TargetMenuP, -1);break;
		case Menu_down: 	TargetMenuPointrt.TargetMenuP=NextCancheMenuList(TargetMenuPointrt.TargetMenuP,  1);break;
		case Menu_Sub:  
			if(TargetMenuPointrt.TargetMenuP->subclass != NULL && TargetMenuPointrt.TargetMenuP->subclass->checked==ENABLE) {
				StatusInformation = Menu_noaction; // ��ֹ�л��˵��б�ʱ�������к����ڲ�ָ��
		  	TargetMenuPointrt.TargetMenuP = TargetMenuPointrt.TargetMenuP->subclass;
			}
			break;
		case Menu_Father:  
			// 0: �������˵�ͷ�ĸ���   1: ��������ǰ�˵��ĸ���
			#if MENU_FATHER
				if(TargetMenuPointrt.TargetMenuP->father != NULL && TargetMenuPointrt.TargetMenuP->father->checked==ENABLE) {
					StatusInformation = Menu_noaction; // ��ֹ�л��˵��б�ʱ�������к����ڲ�ָ��
					TargetMenuPointrt.TargetMenuP = TargetMenu->father;
				}
			#else
				p = FindMeunListHeard(TargetMenuPointrt.TargetMenuP);
				if(p->father != NULL && p->father->checked==ENABLE) {
					StatusInformation = Menu_noaction; // ��ֹ�л��˵��б�ʱ�������к����ڲ�ָ��
					TargetMenuPointrt.TargetMenuP = p->father;
				}
			#endif
			break;
		default:break;
	}
}

// �����˵�ˢ�º�λ
static void MenuRest(void)
{
	RestFont(); //�ָ�Ĭ������
}

//�б�˵�ˢ�º�λ
static void endRest(void)
{

}

// �����ֹʱ
static void StopKeyReceive(uint8_t *p)
{
	MesSIF = Menu_noaction; //��ֹ��ֹʱ����İ���״̬���㲥
	
	if(p == &InuptEnable) //��ֹ��������
	{
		StatusInformationAlways = Menu_noaction;
		StatusInformation = Menu_noaction;
	}
	else if(p == &ResponseEnable) //��������Ӧ
	{
		
	}
}

// ����
static void MenuAlwaysRun_PVH(void)
{
	changeMenu(); //�ֶ��ı�˵�
	
	if(TargetMenuPointrt.LastTargetMenuP != TargetMenu) //������Ϣ�㲥
	{
		cushMes(MesSIF); //�򻺳���д��Ϣ
	}
}
static void MenuAlwaysRun_PLH(void) // 
{
	cushMesBro(Message_Key); //�㲥����������Ϣ
}




/*
	���ܣ��˵�ѡ�з��
*/
static void MenuCheckedStyle(MenuTargetTypedef *MenuPointer)
{
	if(MenuPointer->show == ENABLE) 
	{
		GRAPHICSSHOWMANNER = GraphicsRollColor;
		DrawMenuRectangle(MenuPointer); // �û��˵�ѡ��
		GRAPHICSSHOWMANNER = GraphicsNormal;
	}
}

//�˵�ϵͳ��ʼ��
void MenuSysBaseInit(void) 
{
	// ָʾ����ʼ��
	TargetMenuPointrt.style = DISABLE; //��ʹ��ָ�붯Ч
	TargetMenuPointrt.show = ENABLE;  //ָ����ʾ

	// OLED
	OLED_Init(); // OLED��ʼ��
	
	// �����豸
	KeyDisInit(&MenuKey, 1); // ����������ʼ��
	
}

/*
	���ܣ��˵����к����� ˢ����Ļ ֮ǰ��Խ������Ļ��ʾ���ȼ�Խ��
*/
void MenuRun(void)
{
	EquipmentState(); //�����豸
	FunctionTickerRun();
	
	if(RefreshFlagForHeart == ENABLE){ //ʱ���б�ˢ�±�־
		RefreshFlagForHeart = DISABLE;
		MenuRefresh(0);
	}
	
	if((ScreenPara.refresh) && TargetMenu)
	{
		StateToPointer(); // �豸����״̬�ı�ʵʱĿ��˵�ָ��
		
		ClearnBuff(); // ��ջ���
		MenuAlwaysRun_PVH();
		MenuAlwaysRun_PH();
		MenuAlwaysRun_PLH();
		SpecialFunctionRun(TargetMenu); //���⹦������
		MenuAlwaysRun_PM();
		MenuListOverallRun(TargetMenu); //�˵�ȫ��
		MenuListInterface(TargetMenu); //������ʾ��ǰ�˵��б�
		MenuCheckedStyle(&TargetMenuPointrt);//�˵�ѡ�з��
		MenuAlwaysRun_PL();
		endRest();//ϵͳ��λ
		disp_flush();// ˢ����Ļ
		
		StatusInformation = Menu_noaction; //�����豸״̬��λ
	  ScreenPara.refresh--;// ˢ�±�־��λ
	}
	AlwaysRun();
}








