#ifndef _MENU_TOOL_H_
#define _MENU_TOOL_H_

#include "menu.h"

// *******************************
// *********** �� Ϣ *************
// *******************************

// ��Ϣ״̬ 
enum MessageState
{
	Mes_Have = 0x01, //��Ϣ 
};

// ��Ϣ 
typedef struct MESSAGE
{
	uint32_t message; // ��Ϣ���� 
	uint8_t state;    // ״̬ 
	struct MESSAGE *next; 
}MessageTypedef;





/*
	���ܣ���Ϣ��ʼ��
	Target��Ҫ�������Ϣ�б�ͷ�����ָ�룩 
	mes��Ҫ��ʼ������Ϣ 
*/

MessageTypedef * MessageInit(MessageTypedef** Target, MessageTypedef* mes);

/**
	���ܣ�  ��Ϣ�㲥
	mes��   ��Ϣ������ 
	mesage��Ҫ�㲥����Ϣ 
 **/
void MessageBroadcast(MessageTypedef *mes, uint32_t mesage);


extern MessageTypedef *Message_Key; //������Ϣ�㲥�б� 


#define MesHave(p)  ((p).state & Mes_Have) //�ж� �Ƿ�����Ϣ
#define MesClear(p) ((p).state &= !Mes_Have) // �����Ϣ״̬
#define MesRead(p)  ((p).message)           //����Ϣ


#define MesInit_Key(km) MessageInit(&Message_Key,km)     //������Ϣ - ��ʼ��
//#define MesBro_Key(km) MessageBroadcast(Message_Key, km) //������Ϣ - ����



// --- �� ���½���������Ϣʹ�� ��-----

//���ͻ���
//ע�⣺��Ϣ������Ϊ�ղ���д��ɹ�
void cushMes(uint32_t mesage);

/*
	���ܣ�������Ϣ���㲥����������Ϣ�����ͳɹ���״̬�Զ���λ
	mes����Ϣ�ġ�ͷ�����
	
	ע�⣺���Զ��жϣ������ظ�����
*/
void cushMesBro(MessageTypedef *mes);

/*
	���ܣ��жϻ������Ƿ�Ϊ��
	ret��true �գ�false��Ϊ��
*/
uint8_t cushIsNull(void);

// ���û���Ϊ�� true �� false
void SetCushNull(bool sta);

// ��������
uint32_t readCush(void);









// ************** �� �� �� �� *******************

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
uint8_t ScrollingDisplay_X(menu_area *target, int16_t showSX, int16_t showEX, int16_t TarSX, int16_t TarEX, uint8_t style);






// ************** �� �� �� �� *******************


/*
	���ܣ�������ʾ
	target����ǰ��������һ�˵�ָ�룬 >> �����ο�Ϊ�� <<
	showSY���б�������ʾ����ʼy���� ͷ����
	showEY���б�������ʾ�Ľ���y���� ������
	TarSY�� ָ���������ʼy����     ͷ���� Ӧ >= showSY
	TarEY�� ָ������Ľ���y����     ������ Ӧ <= showEY
	style�� ��αƽ���ʾ   0 ֱ��ˢ����Ŀ��   1 ������ˢ����Ŀ��

	ret: ��������������ɷ���1�����򷵻�0

	ע�⣺style=0ʱ �ú�����ı�����˵���menulistend���ԣ��ڸò˵����ڵĲ˵��б��У����ڳ���˵����⹦�ܣ�
		����ʹ��EnterMenu��ExitMenu��EnterShowMenuList��ExitShowMenuList���ڶ�δ�������
*/
uint8_t ScrollingDisplay_Y(menu_area *target, int16_t showSY, int16_t showEY, int16_t TarSY, int16_t TarEY, uint8_t style);





// *********** �� �� �� �� �� չ �� ****************

/*
	���ܣ��˵��ѵ������£�
	target�����
	mod: 0:��ͷ�˵��ص�  1���뵱ǰ�˵��ص�
*/
void MenuCoorAlignment(menu_area *target, uint8_t mod);

/*
	���ܣ�ʹ�ѵ��Ĳ˵� ��ͷ�˵� ΪĿ��ָ�չ��״̬
	target�����
	mod�� 0 ����չ��  1������չ��
	style: 0 ֱ��չ��   1 ����չ��
	ret : ��ȫչ��������չ������1�����򷵻�0
		
	ע�⣺����չ���󣬿�����ԭ�е����겻һ�£���ʹ������գ�
				������ʹ�˵��������������޹����˵��б�Ҫʹ�ô˺���
 */
uint8_t MenuCoorRecovery(menu_area *target, uint8_t mod, uint8_t style);


/*
	���ܣ��б��л�ʵ�����ж���Ч����
				�б�� չ�� �� ����
	mes����Ϣ��� --��������Ϣ��
	start��ָʾ��������ʼλ��
	end��  ָʾ���������λ��
	dir��  ����0 ����1����

	ע�⣺��ʹ�á����⹦�ܡ���EnterMenu���ѵ��˵� ��MenuCoorAlignment�� ʱ���˺���Ӧ��
			MenuCoorAlignmentִ�е����ȼ��ͣ�����ɷ����� MenuAlwaysRun_PM��
*/
void ListSwitchIns(MessageTypedef *key_mes, bool dir, int16_t start, int16_t end);



/****************** ����PID **********************/

typedef struct TANLEPID
{
	float P;
	float I;
	float D;
	uint16_t maxval; //����ֵ��ֵ
	float last_diff;
	float i_val;
}TanLevPIDTypedef;

/*
	���ܣ�pid��ʼ��
	pidhandle�����
	maxval�����շ���ֵ�ľ���ֵ��ֵ
	P��I��D��pid����
*/
TanLevPIDTypedef *TanLevPIDInit(TanLevPIDTypedef *pidhandle, uint16_t maxval, float P, float I, float D);

/*
	���ܣ����pid����ֵ

	wantval��������ֵ
	nowval��Ŀǰֵ
*/
float TandemLevel_PID(TanLevPIDTypedef *pidhandle, float wantval, float nowval);




// *******************************************
// ************ �� �� �� ʱ ִ �� ************
// *******************************************

enum FUNCTINOTICKEROPTIONS
{
	normalRun = 0,
	interruptRun,
};
typedef struct FUNCTINOTICKER
{
	uint32_t count; //����
	uint32_t ms;  //Ŀ��
	struct FUNCTINOTICKER *next; //��һ��
	void (*Function)(uint32_t d); //ִ�к���
	uint32_t parpass; // ���ݲ���
	enum FUNCTINOTICKEROPTIONS RunMod; //ģʽ
	uint8_t NumOfRun; //���д��� 0�����ƣ� >0 ���޴�����<Ĭ��ֵ 0>
	bool Flag; //ִ�б�־
	bool run; //���� <Ĭ��ʹ��>
}FunctionTicker;

FunctionTicker *SetFunctionTicker(FunctionTicker *FTtarget, uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(uint32_t parpass), uint32_t parpass);
//���ܣ�������ʱִ��
FunctionTicker *AddToFunctionTicker(uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(uint32_t parpass), uint32_t parpass);






// **************************************
// *************** ���� *****************
// **************************************

typedef struct KEYDISPOSE
{
	uint8_t KEYPUTSTA;//�������µ�ƽ 1 - 0
	uint8_t ReadKey;//�������İ���
	uint16_t keypt;//key put time
	uint8_t keynpflag; // key n put flag
	uint8_t keypsc;// key puts count
	uint8_t keylpt;// key long put time
	uint8_t keystate;
	uint8_t keycr; //key can read
	struct KEYDISPOSE *next;
}KeyTypedef;

// ������ʼ��
void KeyDisInit(KeyTypedef *key, uint8_t putsta);
// �������� �ж�
void KeyDisposeISR(uint16_t ms);
// ��ȡ����״̬
uint8_t GetKeyState(KeyTypedef *key, uint8_t *state, uint8_t *keycount);






// **********************************************
// ****************** �������� ******************
// **********************************************

typedef struct RUNONE
{
	uint32_t dat;
	uint8_t floag;
}TypeRunOne; //���ʼ��Ϊ {1, 0};

/* t=0: dat�������һ��  t=1��dat2�ı�����һ��!!!ֻ�ܱ�����һ��!!! */
bool tRunOne(TypeRunOne *RunOne, bool t, uint32_t dat1,uint32_t dat2);









// ****** �� �� **********


//�ڴ�����
void ClearnMemory(void *m, uint16_t size);

// ����ֵ
uint32_t myabs(int32_t dat);



#endif



