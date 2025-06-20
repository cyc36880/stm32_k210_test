#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>  //uint16_t �����ڵ�
#include <stdlib.h>  //malloc   ���ڵ�
#include <stdbool.h> //bool     �������ڵ�

#include "oled.h"
#include "menuconfig.h"

/*
�ر�ע�⣺

	�������ж���ʹ�����º���������
		
	ÿ��������ϸ��ע�������Լ�����ֵ�͹��ܵȣ�����ת�ú����ϲ��鿴
	
	�ò˵�����ʹ��malloc����ռ䣬��free��ע���ڴ��������ȷ��heap(��)��С�㹻
*/

// ================================ �� �� ==============================

#define MENUHEARDID 1 //�˵��б���ʼID

#ifndef ENABLE
	#define ENABLE  1 //ʹ��
#endif
#ifndef DISABLE
	#define DISABLE 0 //ʧ��
#endif

// �˵�����
enum MenuState
{
	Menu_noaction=0, // �޲���
	Menu_up,  		 // ����
	Menu_down, 		 // ����
	Menu_Sub,        // ����
	Menu_Father,     // ����
	Menu_confirm 	 // ȷ��
};

// �û�������Ϣ
enum SpecialInformation
{
	MenuTime = 0x01, //ʱ���б�
	EnterMenu = 0x02, // ����˵���ifĩβ�����return 
	ExitMenu = 0x04,  // �˳��˵���ifĩβ�����return
	EnterShowMenuList = 0x08, // ������ʾ�˵��б�ifĩβ�����return ������ʾ���øù���
	ExitShowMenuList = 0x10, // �˳���ʾ�˵��б�ifĩβ�����return  ������ʾ���øù���
	
	NotIgnore = 0x20, //������ʾ�ķ�Χ����Ȼˢ�¡���ע�ᣬ���ɼ��
	
	/*****�������ܵĸĽ�*****/
	
	MenuTimeForce = 0x40, //������ʱ���б�ǿ��ִ�С� ��MenuTime���ʹ�ã�������Ч������Դ��жϣ�ifĩβ�����return������
	/*
		������ ע��ʱ�����Ӹñ�־��������˵���ʾʱ��������������
		��������ñ�־�����жϵ�ȡ��Ӧ����������ֻҪ���ڸò˵��������б��У�����������������������Ӧ����
	*/
	
	/******** �� �� **********/
	MenuHaveOverall = 0x80,//����ʹ�ã���������ռ�ø�λ���˵��б�ȫ��
	MenuScrollingY  = 0x100,//����ʹ�ã���������ռ�ø�λ���˵��б�Y����
	MenuScrollingX  = 0x200,//����ʹ�ã���������ռ�ø�λ���˵��б�X����
};




//�˵�ʱ�����
typedef struct MENU_TIMEMS
{
	uint16_t counttime; //��ʼ����ֵ
	uint16_t timems;    //Ԥ��ʱ��
}menu_timems;
//�б��ʼ��
typedef struct LISTINIT
{
	void *Target;
	struct LISTINIT *next;
	uint8_t flag;
}ListInit;

//�˵�����
typedef struct MENU_AREA
{
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t high;
	uint16_t id; //�˵�ID����Ҫ�޸�
	bool checked; //�ܷ�ѡ�� 
	bool menulistend; //�˵��б����
	struct MENU_AREA *next;     //��һ��
	struct MENU_AREA *previous; //��һ��
	struct MENU_AREA *subclass; //����
	struct MENU_AREA *father;   //����
	void (*menuinterface)(struct MENU_AREA *target); //�˵�����
	menu_timems *menu_time;
	uint16_t specialfeatures;//���⹦��ע��
	uint16_t specfeattrigflag; //���⹦�ܴ������
	
	ListInit ListTarget; //��ʼ���б�
}menu_area;


typedef struct MENUTARGET //�˵�ָʾ��
{
	menu_area * TargetMenuP;
	menu_area * LastTargetMenuP;
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t high;
	uint8_t R;  //Բ��
	bool style; //����
 	bool show;  //��ʾ
}MenuTargetTypedef;

//�б�ȫ�ֶ���
typedef struct MENULISTOVERALL 
{
	menu_area *Affiliation; //������
	void (*menuinterface)(void); //�˵�����
	struct MENULISTOVERALL *next;
}MenuListOverall;





// ----------- �� �� Ѱ ַ --------------------



//���ܣ��Բ˵�����ƫ��Ѱַ 
menu_area *MenuListAddressing(menu_area *target, bool upordown, uint16_t offset);

//���ܣ��Բ˵�ͷǰ��ƫ��Ѱַ 
menu_area *MenuHeadAddressing(menu_area *target, bool upordown, uint16_t offset);

//���ܣ�Ѱ����Ŀ�굥ԪΪ��׼�ĵ�num����ѡ�еĲ˵������򷵻�ԭ��ַ
menu_area *NextCancheMenuList(menu_area *target, int16_t num);

//���ܣ�Ѱ����Ŀ�굥ԪΪ��׼�ĵ�num����ѡ�еĲ˵�ͷ�����򷵻�ԭ��ַ
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num);

//���ܣ��ҵ��˵����ڲ˵��б�Ĳ˵�ͷ����ָ��NULL 
menu_area *FindMeunListHeard(menu_area *target);

//���ܣ��ҵ��˵����ڲ˵��б�Ĳ˵�β����ָ��NULL
menu_area *FindMeunListTail(menu_area *target);

//���ܣ����ص�ǰ��ʾ�б��ͷ����ʹ�����ܱ�ѡ��
menu_area *MenuListShowHead(menu_area *target);

//���ܣ����ص�ǰ��ʾ�б��β����ʹ�����ܱ�ѡ��
menu_area *MenuListShowTail(menu_area *target);

//���ܣ��ҵ�ΪID�Ĳ˵�  mod��0�����򷵻�ԭָ�룬1�����򷵻�NULL
menu_area *FindMenuOfID(menu_area *target, uint16_t id, bool mod);



// ---------- �� �� ע �� �� �� �� -----------------



//���ܣ�ע�����Ӳ˵� 
menu_area *AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);
//���ܣ������в˵�ע�����Ӳ˵� 
menu_area *SetMenu(menu_area *target, int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);


//���ܣ�����Ŀ��˵��·�����  	mod������ 0:�г���������������ͷ���� 1:ֻ����ȫ����Ļ�·��Ű�ͷ���� 2:��������·�����
menu_area *FastSimilarMenuDown(menu_area *target, menu_area *source, uint8_t mod);
//���ܣ�����Ŀ��˵��Ҳ����  	mod������ 0:�г���������������ͷ���� 1:ֻ����ȫ����Ļ�ҷ��Ű�ͷ���� 2:��������ҷ�����
menu_area *FastSimilarMenuRight(menu_area *target, menu_area *source, uint8_t mod);


//���ܣ�������ʼ��  source��menu_area���͵�����  n���������  mod������  menuinterface��ͳһ���ӵĺ���
void BatchFastSimilarMenuDown(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target));
//���ܣ�������ʼ��  source��menu_area���͵�����  n���������  mod������  menuinterface��ͳһ���ӵĺ���
void BatchFastSimilarMenuRight(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target));


//���ܣ����ӵ�����
void LinkToParentClass(menu_area *father, menu_area *sub);


//���ܣ�Ŀ��˵���β��������ָ������
void MakeMenuListRing(menu_area *target);



// --------------- �� ʼ �� �� �� -------------


// ���ܣ����б�����ʼ��ִ�к���
ListInit* AddToListInit(ListInit **ListTarget, ListInit *target, void *func);

//���ܣ��б���  ע�⣺if ĩβ��  return����
uint8_t CheckListInit(ListInit *ListTarget);


// ----- �� �� �� �� ---


//���ܣ����뵽�˵���ʼ���б���
void AddToMenuListInit(menu_area *target);

//���ܣ���ʼ�˵��б��ʼ��
void sMenuListInit(void);

//���ܣ��˵��б���    ע�⣺if ĩβ��  return����
uint8_t CkeckMenuList(menu_area *target);




// ************* �� �� �� �� *******************



//���ܣ��˵��б�ʼ��ִ�к��� <���Ե�ǰ�б�>
MenuListOverall *MenuOverall(menu_area *target);


//���ܣ������е�ʱ��������˵� <�����ʱ������⹦��ע��>
void SetMenuTime(menu_area *target, menu_timems *menutime_obj, uint16_t function, uint16_t ms);

//���ܣ����⹦��ע��
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms);

//���ܣ����⹦�ܼ�飬��������1�����򷵻�0
bool TriggerCheck(menu_area *target, enum SpecialInformation function);


//���ܣ��ֶ�ָ��Ҫ�ı�Ĳ˵������㲥��Ϣ target��ָ���Ĳ˵�ָ�� mes����Ϣ
void gotoMenu(menu_area *target, enum MenuState mes);




extern MenuTargetTypedef TargetMenuPointrt;// ʵʱĿ��˵�
#define TargetMenu TargetMenuPointrt.TargetMenuP

extern enum MenuState StatusInformation; //�����豸״̬
extern enum MenuState StatusInformationAlways; // �����豸״̬ <����ı�>

extern uint8_t InuptEnable; //����ʹ��
extern uint8_t ResponseEnable; //�˵��л���Ӧ




/*
* ����Ҫ��ʾռ�õĸ߶ȣ�high�����ȣ�width����������Ͻǵ�x��y�����
* �˵����ĵ�����
*/
#define MCX(target, w) ((target->width - (w)) / 2)
#define MCY(target, h) ((target->high - (h)) / 2)



void MenuAlwaysRun_PH(void);
void MenuAlwaysRun_PM(void);
void MenuAlwaysRun_PL(void);
void MenuSysBaseInit(void); //�˵�ϵͳ��ʼ��
void AlwaysRun(void);









// =========================== �� Ļ ====================================


#define SCREENWIDTH 128 // ��Ļ���
#define SCREENHIGH  64  // ��Ļ�߶�

//�������� x=8 -> 1, x=9 -> 2
#define DIVIDEUP(x) (x*10/8%10 ? x/8+1:x/8)

enum ScreenShowManner //��Ļ��ʾ��ʽ
{
	ScreenNormal,  //����
	ScreenRollback //��ת
};

enum GraphicsShowManner //ͼ����ʾ��ʽ
{
	GraphicsNormal,  //����
	GraphicsColless, //��ɫ
	GraphicsCover,   //����
	GraphicsRollColor //��ɫ����ȡĿ��㣬ȡ����
};

//��Ļ����
typedef struct
{
	const uint16_t screenwidth; //��Ļ���
	const uint16_t screenhigh;  //��Ļ�߶� (���ϳ�8)
	uint8_t refresh; //ˢ�±�־
}TypedefScreen;

extern enum ScreenShowManner SCREENSHOWMANNER; //��Ļ��ʾ��ʽ
extern enum GraphicsShowManner GRAPHICSSHOWMANNER;//ͼ����ʾ��ʽ

extern unsigned char DisplayBuff[]; // ��Ļ��ʾ����
extern TypedefScreen ScreenPara; // ��Ļ�������

//���ܣ���Ļ���ˢ��ע��  mod  0�������ڱ��е�ˢ�´��������򴴽�  ����������N��ˢ��
void MenuRefresh(uint32_t mod);
// ���ܣ���Ŀ��˵������λ�û���
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b); 
//�����ʾ����
void ClearnBuff(void);


/*
	���ܣ��жϲ˵��Ƿ�����Ļ��

	ret��0���� 1��
*/
uint8_t inScreen(menu_area *target);


/*
* ����Ҫ��ʾռ�õĸ߶ȣ�high�����ȣ�width����������Ͻǵ�x��y�����
* ��Ļ���ĵ�����
*/
#define MSCX(width) ((SCREENWIDTH - (width)) / 2)
#define MSCY(high)  ((SCREENHIGH - (high)) / 2)







// ============================== �� �� ==================================


/*
	���ܣ��˵�����ռ�
	size������ռ���ֽ���
	@ret������ռ���׵�ַ��ʧ��Ϊ��
*/
void *MenuMalloc(uint16_t size);

extern uint32_t MenuMallocSize;//�˵�����Ŀռ��С


// ���ܣ��˵����к�����Ϊ��֤�������У��ú�����while��ÿ��ѭ������Ӧ����2000��
void MenuRun(void); 
// ���ܣ��˵�����ִ�У�ÿ1msִ�иú���
//ms ���������
void MenuTicker_ms(uint16_t ms);
//�˵�������ʼ��־ ������� ��ENABLE����
extern bool MenuHeartTimeStart; 




#endif





