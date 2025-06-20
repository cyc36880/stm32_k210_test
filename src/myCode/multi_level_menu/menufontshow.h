#ifndef _menufontshow_h_
#define _menufontshow_h_

#include "menu.h"
#include "menuconfig.h"

typedef struct FONTINFO
{
	uint16_t ascnum; //Ӣ������
	uint16_t hznum;  //��������
	uint16_t maxPix; //���һ������ֵ
	uint8_t ascsize[2];//Ӣ�ĳߴ�
	uint8_t hzsize[2]; //���ֳߴ�
	uint8_t LBnum; //��������
}FontInfoType;


extern const uint8_t *font_SizeInf; //�����С

// ����Ӣ����ʾ����
void SetFont(const uint8_t *xfont);

//��λĬ������
void RestFont(void);


// -- ������ʾ --

// ���ܣ��ڲ˵���дAsc�ַ�����targetΪNULL����Ļд
FontInfoType* MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str);
// ���� Asc �����ʾ��targetΪNULL����Ļд
FontInfoType* MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat);

// -- ������ʾ --

// �ı���ʾ <��֧��Ӣ��>   <ascw �ַ���������>
FontInfoType* CharacterText(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);
// �ı���ʾ <��Ӣ�Ļ����ʾ> �м������������߶�ִ�У��� <ascw ����>
FontInfoType* CharacterTextC(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);


// ���ܣ��ڲ˵���дAsc���֣�targetΪNULL����Ļд
FontInfoType * MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num);


/*
	���ܣ���ʹ��printf����Ļ��ӡ�ַ�
	target��Ŀ��˵�
	mod��0 �ڲ�����CharacterText����֧�����ģ����������
			 1 �ڲ�����CharacterTextC��֧�����ģ����м�����п�϶
	x,y: ����
	formate��..: ��printf��ͬ
	ret��������ַ�����
	ע�⣺�ڲ�����̶��ڴ�100�ֽڣ���Ҫ���̫���ַ���
*/
FontInfoType* m_printf(menu_area *target, int16_t x, int16_t y, const char *format, ...);

/*
	���ܣ����ֹ�����ʾ y���� ��������
	target���˵������ΪNULLû�����壩
	x��y ������ڲ˵�������
	mem�������¼λ�ã����ʼ��Ϊ0
	now_nub����ǰ��ֵ
	want_nub��������ֵ
	speed: �����ٶ�
 */
void NumScrollShow(menu_area *target, int16_t x, int16_t y, uint8_t *mem, uint8_t *now_nub, uint8_t want_nub, uint8_t speed);
void NumScrollSet(const uint8_t *pic, uint16_t zf_x, uint16_t zf_y);

/*
	���ܣ��ַ�������
	
	str���ַ����� \0 ��β��
	
	ret����������Ϣ
*/

FontInfoType* StringDeal(const uint8_t *str);

/*
	���ܣ��Զ�����ָʾ���ߴ�

	FontInfo��������Ϣ���
	target��ָʾ�����
	mod�� 0��ʹ�õĺ����������޹�  1��������������
	limitSize���ߴ����ƣ������ÿ���NULL������Ϊ{��խ����� ������}��ĳλ�����ƿ�����0
	ret�������ߴ��޸�ʱ������1�� ���򷵻�0
*/
uint8_t SetIndicatorSize(FontInfoType *FontInfo, menu_area *target, const uint8_t *limitSize);

// -------------- �� �� --------------


#define MENUSERIALWINDOWWIDTH   21 //���ڿ�ȣ��ֽ�Ϊ��λ max:255
#define MENUSERIALWINDOWHIGH   5 //���ڸ߶ȣ��ֽ�Ϊ��λ   max:255

#define MenuSerialBufSzie  (MENUSERIALWINDOWWIDTH * MENUSERIALWINDOWHIGH + 1) //���ڻ������ֽڴ�С


extern uint16_t MenuSerialRxNum; //�������������ΪMenuSerialBufSzie


//�������
void MenuPaddingSerial(uint8_t rx);
// �����ı���ʾ<��֧��Ӣ��>
void SerialCharacterText(menu_area *target, int16_t x, int16_t y);
//��մ�����ʾ
void ClearnSerialShowBuf(void);

void MenuSerStopRecive(uint8_t enb);
uint8_t SerisStop(void);

void SerSetHex(uint8_t isHex);
uint8_t SerisHex(void);
/*
	�����ַ���ƥ��
	@ret 1:����ƥ���ַ� 0��ƥ��ʧ��
*/
uint8_t SerStrMatching(const uint8_t *str);
	

#endif



