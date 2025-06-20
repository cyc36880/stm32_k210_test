#ifndef _Graphicalfunctions_h_
#define _Graphicalfunctions_h_

#include "menu.h"
#include "menu_tool.h" //����

// ����
void write_point(int16_t x, int16_t y, uint8_t w_d);
// ����
unsigned char read_point(int16_t x, int16_t y);

//��8x128�������ﻭ��
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

//����Բ
void DrawCircle(int16_t x0,int16_t y0,uint8_t r);
//ʵ��Բ
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r);

//���ľ���
void DrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h);
//ʵ�ľ���
void DrawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h);

//����Բ�Ǿ���
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
//ʵ��Բ�Ǿ���
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);

//����α任
void PolygonTransformation(int16_t x0,int16_t y0,uint16_t r, uint16_t n);

//��תĿ���
void RotateXY(int *xy, int centerX, int centerY, int x, int y, int Angle, char direct);

//�������� cx��cy����������  w��h���  rot����0.1Ϊ��λ�������ɣ������ƣ�
void Linecube(int cx, int cy, int w, int h, double rot);

//�������� centerX��centerY �������� �� size���߳� �� rotx roty rotz����xyz��ĽǶȣ������ƣ�
void DrawCube(int centerX, int centerY, int size, float rotX, float rotY, float rotZ);





//------------------  �� �� ͼ -------------------------



typedef struct LineChartMap
{
	const uint8_t width; //��� 0-128
	const uint8_t high;  //�߶� 0-128
	const uint16_t max;  //ȡֵ��Χ
	uint8_t Startp; //��ʼ��ʾλ�� 
	uint8_t Endp;   //������ʾλ��
	uint8_t RxNum;
	uint8_t dat[128];
}TypLineChartMap;
//����ʵ��
//TypLineChartMap p = {width, high, max};

//����ͼ�ڴ����� <��ʼ���ڴ�>
void ClearnLineChartMapDat(TypLineChartMap *t);
//������ͼ���������
void AddDatToLineChartMap(TypLineChartMap *t, uint16_t d);
//���ܣ�����ͼ���� @ret�����һ������������ͼ�е���ʾ����
int16_t *LineChart(TypLineChartMap *t, int16_t x, int16_t y);






// ---------------- ͼ Ƭ -------------------


/*
	���ܣ�ͼƬ��ʾ
	ȡģ��ʽ������ ����ʽ ����

	ע�⣺��psizeΪNULL��ͼƬ��СӦ������p�У������С����psize���ã���ʵ��С���ú�����������ͼƬ��
*/
void DrawPicture(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y);
/*
	���ܣ�ͼ��������ʾ
	psize��ԭʼ�ߴ���Ϣ��ΪNULL��ʹ��p��ͷ��λ
	x��y���������Ļ������
	zf_x��zf_y:x��y��������ű�����100Ϊԭ�ߴ�
*/
void ImageScaling(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y, uint16_t zf_x, uint16_t zf_y);


// -------------------- �� �� �� -----------------------

enum PROGRESSBAR_ATTR
{
	hideframe     = 0x01,  //���ر߿�
	hideindicator = 0x02,  //����ָʾ��
};

typedef struct  PROGRESSBAR
{
	uint16_t BarVal;    //�洢����
	uint16_t pixVal;    //������������Ϣ
	uint16_t maxVal;    //�������ֵ
	uint8_t width;     //���
	uint8_t high;       //�߶�
	uint8_t R;          //Բ��
	uint8_t attribute;  //����
	bool animation;     //����
	uint8_t direction;  //���� 0-3
	TanLevPIDTypedef pidhandle; //pid
}ProgressBarTypedef;

/*
	���ܣ���������ʼ��
	barobj : ���
	direction : 0-3�� ��ʾ����˳ʱ����ת
	weight : ��ȣ�������Ļ
	high   : �߶ȣ�������Ļ
	maxVal : ��������ʱ���ֵ
	
*/ 
ProgressBarTypedef * ProgressBarInit(ProgressBarTypedef *barobj, uint8_t direction, uint8_t width, uint8_t high, uint16_t maxVal);
/*
	���ܣ���������ʾ
	barobj�����
	x��x����
	y��y����
	val������ֵ����maxVal�Ƚϼ���������
	
	ret: ָʾ��ĩ����������

	ע�⣺Ĭ���޶�Ч��������ʹ�� animation ������ʱ����
*/
int16_t * ProgressBar(ProgressBarTypedef *barobj, int16_t x, int16_t y, uint16_t Val);


//���ܣ���ȡbar��ֵ
uint16_t getBarVal(ProgressBarTypedef *barobj);

//��������������
void SetProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute);

//���������Ը�λ
void ResProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute);






// ----------------- ģ �� --------------------- 


/*
	���ܣ�oled����ģ��
	n: 0-4  0��ģ�� >=4��������
*/
void InterfaceBlurry(uint8_t n);

/*
	���ܣ��˵����롢�뿪�黯
	state: 0 �˵���ͣ  1 �˵��黯
	
	ret��1 ��ʼ�黯�����ڵ�ǰ�˵���  0 ��ʼʵ���������л���Ĳ˵���

	ע�⣺ ...
				�����黯ʱ ������Ӧ ����ֹ���ڲ������ı�ָ��ʱ�������ж��Ƿ��� ������Ӧ��
				�ٸı�˵�ָ��
				state=0 ʱ������Ӧ����   MenuAlwaysRun_PH �����ʼ����
				state=1 ʱ�������������� MenuAlwaysRun_PL ����������
*/
uint8_t MenuDynamicBlurry(uint8_t state);

#endif








