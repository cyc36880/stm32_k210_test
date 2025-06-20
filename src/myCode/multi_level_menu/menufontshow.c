#include <stdarg.h>
#include <stdio.h>

#include "menufontshow.h"
#include "Graphicalfunctions.h"
#include "oledfont.h"

static uint8_t fontchangeflag=0;
static const uint8_t *font = F8X16+2;
const uint8_t *font_SizeInf = F8X16;

//����Ӣ����ʾ����
void SetFont(const uint8_t *xfont)
{
	fontchangeflag = 1;
	font = xfont+2;
	font_SizeInf = xfont;
}

//��λĬ������
void RestFont(void)
{
	if(fontchangeflag)
	{
		font = F8X16+2;
		font_SizeInf = F8X16;
	}
}

/*
	���ܣ��ڲ˵���дAsc�ַ���targetΪNULL����Ļд
	target��Ŀ��˵�
	font����������
	font_SizeInf������������
	x,y����Ŀ��˵������ƫ��
	asc��һ��ASCII�ַ�
*/
static void MenuShowAsc(menu_area *target,int16_t x, int16_t y, uint8_t asc)
{
	uint8_t c=0;
	uint8_t h,w;
	bool w_b=0;
	uint8_t wight = font_SizeInf[0];
	uint8_t high  = font_SizeInf[1];
	
	if(asc > '~' || asc < ' ') asc = ' '; //������ʾ��Χ
	c=asc-' ';   // ' '=32,ASCII���
	
	
	if(target == NULL) 
	{
		DrawPicture(NULL,font_SizeInf, font + c*wight*DIVIDEUP(high), x, y);
	}
	else
	{
		if( (x+font_SizeInf[0]-1)<0 || (y+font_SizeInf[1]-1)<0 || x>=target->width || y>=target->high) //������ʾ����
			return;
		
		if( x>=0 && y>=0 && (x+font_SizeInf[0]-1)<target->width && (y+font_SizeInf[1]-1)<target->high)
		{
			DrawPicture(NULL, font_SizeInf, font + c*wight*DIVIDEUP(high), target->x + x, target->y + y);
		}
		else
		{
			int16_t start_x, start_y;
			int16_t end_x, end_y;
			
			start_x = x < 0 ? -x : 0;
			start_y = y < 0 ? -y : 0;
			
			end_x = (x+wight-1)<target->width ? wight : target->width - x;
			end_y = (y+high-1)<target->high ? high :target->high - y;
			
			for(h=start_y; h<end_y; h++)//�ָ�
			{
				for(w=start_x; w<end_x; w++) //�ֿ�
				{
					w_b = ( font[c*wight*DIVIDEUP(high) + w+wight*(h/8)] >> (h%8) ) & 1;
					if(w_b) {
						MenuSetPoint(target, w+x, h+y, 1);
					}
					else { //������ʾ
						if(GRAPHICSSHOWMANNER == GraphicsCover) {
							MenuSetPoint(target, w+x, h+y, 0);
						}
					}
				}
			}
		}
	}
}
//m^n����
static uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

/*
	���ܣ��ڲ˵���дAsc���֣�targetΪNULL����Ļд
	target��Ŀ��˵�
	font����������
	font_SizeInf������������
	x,y����Ŀ��˵������ƫ��
	len�����ֳ��� 0�Զ�
	num������
*/
FontInfoType * MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num)
{     
	static FontInfoType fontinfo;
		
	uint8_t t,temp;
	uint8_t enshow=0;	
	uint32_t nump = num;
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //���
	
	if(len == 0) { //len=0 , �Զ����㳤��
		if(nump == 0) {
			len = 1;
		}
		while(nump) {
			len++;
			nump/=10;
		}
	}
	
	fontinfo.ascnum = len; //��Ϣ����
	
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				MenuShowAsc(target, x+font_SizeInf[0]*t, y, ' ');
				continue;
			}else enshow=1; 
		}
		MenuShowAsc(target, x+font_SizeInf[0]*t, y, temp+'0');
	}
	
	fontinfo.ascsize[0] = font_SizeInf[0];
	fontinfo.ascsize[1] = font_SizeInf[1];
	fontinfo.maxPix = fontinfo.ascnum * fontinfo.ascsize[0];
	return &fontinfo;
}

/*
	���ܣ��ڲ˵���дAsc�ַ�����targetΪNULL����Ļд
	target��Ŀ��˵�
	font����������
	font_SizeInf������������
	x,y����Ŀ��˵������ƫ��
	str��һ��ASCII�ַ���
*/
FontInfoType* MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str)
{
	static FontInfoType fontinfo;
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType));
	
	while(*str != '\0')
	{
		MenuShowAsc(target, x, y, *str);
		str++;
		x+=font_SizeInf[0];
		fontinfo.ascnum++;
	}
	
	fontinfo.ascsize[0] = font_SizeInf[0];
	fontinfo.ascsize[1] = font_SizeInf[1];
	fontinfo.maxPix = fontinfo.ascnum * fontinfo.ascsize[0];
	
	return &fontinfo;
}


/*
	���ܣ�Ӣ����ʾ�ı�

target���˵�ָ��
x���ı������Ͻ�x����
y���ı������Ͻ�y����
ascw����ʾ��� ���ַ�����������
str����ʾ�����ݴ�
ע�⣺����\nΪ�س�������δʶ��������ʾΪ�ո񣬲�֧��������ʾ����
*/

FontInfoType* CharacterText(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str)
{
	static FontInfoType fontinfo;
	uint16_t maxPix = 0;
	
	uint8_t showbuf;
	uint16_t strnum = 0;
	uint16_t showx=0, showy=0;
	const uint8_t *s = str;
	uint16_t num = 0;
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType));
	
	while(*s++) {
		num++;
	}

	if(ascw == 0) {
		ascw = 65535;
	}
	
	while(num)
	{
		num--;
		showbuf = str[strnum++];
		if(showbuf == '\n') {
			showx = 0;
			showy++;
			
			fontinfo.LBnum++;//�ַ����ݸ���
			if(maxPix >fontinfo.maxPix ) fontinfo.maxPix = maxPix;
			maxPix = 0;
			
			continue;
		}
		MenuShowAsc(target,x + showx*font_SizeInf[0],  y + showy*font_SizeInf[1], showbuf);

		if(++showx >= ascw) {
			showx = 0;
			showy++;
			
			fontinfo.LBnum++;//�ַ����ݸ���
			if(maxPix >fontinfo.maxPix ) fontinfo.maxPix = maxPix;
			maxPix = 0;
			
		}
		fontinfo.ascnum++;
		maxPix+=font_SizeInf[0];
	}
	
	fontinfo.ascsize[0] = font_SizeInf[0];
	fontinfo.ascsize[1] = font_SizeInf[1];
	if(maxPix >fontinfo.maxPix ) fontinfo.maxPix = maxPix;
	return &fontinfo;
}




// ---------------- �� �� �� ʾ------------------------


/*
	���ܣ�������\0Ϊ��β���ַ�������
*/
static unsigned int mystrlen(const uint8_t *str)
{
	unsigned int i=0;
	for(i=0; str[i] != '\0'; i++);
	return i;
}

///*
//	���ܣ�������\0Ϊ��β�� �������ַ� ���ַ�������
//*/
//static unsigned int HZStrLen(uint8_t *str) 
//{
//	unsigned int StrLen = mystrlen(str);
//	unsigned int i=0;
//	unsigned int strnum=0;
//	for(i=0; i<StrLen;)
//	{
//		if((unsigned char)str[i]>127) 
//			i+=2;
//		else
//			i++;
//		strnum++;
//	}
//	return strnum;
//}

///*
//	���ܣ�����֪�ַ���������Ҫ���ַ���λ��
//	formfind��Ŀ���ַ���
//	find����ҪѰ�ҵ��ַ����ڵ��ַ���
//	num��find�е�num���ַ�
//	����ֵ��Ѱ���ַ���formfind�е�λ��
//*/
//static unsigned char StrIfHaveStr(const uint8_t *formfind, uint8_t *find, unsigned int num)
//{
//	unsigned int findLen = mystrlen(find);
//	unsigned int formfindlen = mystrlen(formfind);
//	unsigned int i=0, j=0;
//	unsigned int strnum=0, HZflag=0;
//	
//	if(num >= findLen) return 0;
//	if(formfindlen==0 || findLen==0) return 0;
//	for(strnum=0, i=0; strnum<num+1; )
//	{
//		if((unsigned char)find[i]>127){
//			i+=2;
//			if(strnum==num) HZflag=1;
//		}
//		else{
//			i++;	
//		}
//		strnum++;
//	}
//	for(strnum=0, j=0; strnum<formfindlen; )
//	{
//		strnum++;
//		if(HZflag)
//		{
//			if(formfind[j]==find[i-2] && formfind[j+1]==find[i-1]) 
//				return strnum;
//		}
//		else
//		{
//			if(formfind[j]==find[i-1])
//				return strnum;	
//		}
//		if((unsigned char)formfind[j]>127){
//			j+=2;
//		}
//		else{
//			j++;	
//		}	
//	}
//	return 0;
//}
//// ������ʾ��targetΪNULL����Ļд
//static void MenuHZ16x16Str(menu_area *target, int16_t x, int16_t y, uint8_t *s_dat)
//{
//	unsigned char i0=0, s_datlen = HZStrLen(s_dat);
//	unsigned char j0=0;
//	
//	uint8_t i,j;
//	
//	for(i0=0; i0<s_datlen; i0++)
//	{
//		j0=StrIfHaveStr(HZK16x16index, s_dat, i0);
//		if(j0 == 0) {
//			continue; // û���ҵ�
//		}

//		for(j=0; j<16; j++) //�߶�
//		{
//			for(i=0; i<16; i++) //���
//			{
//				if((HZK16x16[(j0-1)*2 + j/8][i]) & (0x01<<(j%8))){
//					if(target == NULL) write_point(x+i, y+j, 1);
//					else               MenuSetPoint(target,x+i, y+j, 1);
//				}
//				else {
//					if(GRAPHICSSHOWMANNER == GraphicsCover) {
//						if(target == NULL) write_point(x+i, y+j, 0);
//						else               MenuSetPoint(target,x+i, y+j, 0);
//					}
//				}
//			}
//		}
//	}
//}

/*
	���ܣ������������ַ�������ȥascii��λ��
*/
static unsigned int HzInStr(const uint8_t *formfind, const uint8_t *find) 
{
	unsigned int fontnum =  mystrlen(formfind);
	unsigned int i=0;
	unsigned int fontn = 0;
	
	if(fontnum<2 || mystrlen(find)!=2 || find[0]<=127) {
		return 0;
	}
	
	for(i=0; i<fontnum-1; i++)
	{
		if(formfind[i] > 127) {
			fontn++;
			if(formfind[++i]==find[1] && formfind[i-1]==find[0]) {
				return fontn;
			}
		}
	}
	return 0;
}
// ����������ʾ��targetΪNULL����Ļд
static void MenuHZ16x16(menu_area *target, int16_t x, int16_t y, uint8_t *s_dat)
{
	unsigned int j0=0;
	unsigned char i,j;
	
	const uint8_t psize[] = {16, 16};
	
	
	j0=HzInStr(HZK16x16index, s_dat);
	if(j0 == 0) {
		return; // û���ҵ�
	}
	
	if(target == NULL) 
	{
		DrawPicture(NULL, psize, (const uint8_t *)HZK16x16[(j0-1)*2], x, y);
	}
	else
	{
		if( x>=0 && y>=0 && (x+16-1)<target->width && (y+16-1)<target->high)
		{
			DrawPicture(NULL, psize, (const uint8_t *)HZK16x16[(j0-1)*2], target->x+x, target->y+y);
		}
		else
		{
			int16_t start_x, start_y;
			int16_t end_x, end_y;
			
			start_x = x < 0 ? -x : 0;
			start_y = y < 0 ? -y : 0;
			
			end_x = (x+16-1)<target->width ? 16 : target->width - x;
			end_y = (y+16-1)<target->high ? 16 :target->high - y;
			
			for(j=start_y; j<end_y; j++) //�߶�
			{
				for(i=start_x; i<end_x; i++) //���
				{
					if((HZK16x16[(j0-1)*2 + j/8][i]) & (0x01<<(j%8))){
						MenuSetPoint(target,x+i, y+j, 1);
					}
					else {
						if(GRAPHICSSHOWMANNER == GraphicsCover) {
							MenuSetPoint(target,x+i, y+j, 0);
						}
					}
				}
			}
		}
	}
}

// ���� Asc �����ʾ��targetΪNULL����Ļд
FontInfoType* MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat)
{
	static FontInfoType fontinfo;	
	
	unsigned int s_datlen = mystrlen(s_dat);
	unsigned int i=0;
	unsigned char HZShowBuf[] = {0,0,0}; // ������λ����0��β
	uint8_t offset = font_SizeInf[1] < 16 ? 0 : font_SizeInf[1] - 16; //����ƫ����
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //���
	
	for(i=0; i<s_datlen; )
	{
		if(s_dat[i] > 127) //����
		{
			HZShowBuf[0]=s_dat[i];
			HZShowBuf[1]=s_dat[i+1];
			MenuHZ16x16(target, x, y + offset, HZShowBuf);
			x+=16; // �������� �ɺ��ֿ�Ⱦ���
			i+=2;  // �ַ���λ�ñ�־���ƣ����̶ֹ�Ϊ2
			fontinfo.hznum++;
		}
		else   //Asc 
		{
			MenuShowAsc(target, x, y + offset + (16 - font_SizeInf[1]), s_dat[i]);
			x+=font_SizeInf[0]; //�������� ��Asc��Ⱦ���
			i++;  //�ַ���λ�ñ�־���ƣ�Asc�̶�Ϊ1
			fontinfo.ascnum++;
		}
	}
	
	fontinfo.ascsize[0] = font_SizeInf[0];
	fontinfo.ascsize[1] = font_SizeInf[1];
	fontinfo.hzsize[0] = 16;
	fontinfo.hzsize[1] = 16;
	fontinfo.maxPix = fontinfo.hznum*fontinfo.hzsize[0] + fontinfo.ascnum*fontinfo.ascsize[0];
	return &fontinfo;
}

/*
	���ܣ�������ʾ�ı�

target���˵�ָ��
x���ı������Ͻ�x����
y���ı������Ͻ�y����
ascw����ʾ��� <����>
str����ʾ�����ݴ�
ע�⣺����\nΪ�س�������δʶ��������ʾΪ�ո��м������������߶�ִ�У���
*/
FontInfoType* CharacterTextC(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str)
{
	static FontInfoType fontinfo;	
	uint16_t maxPix = 0;
	
	uint8_t showbuf[3] = {0, 0, 0};
	uint16_t strnum = 0;
	uint16_t showx=0, showy=0;
	const uint8_t *s = str;
	uint8_t offset = font_SizeInf[1] < 16 ? 16 : font_SizeInf[1]; //�ı�����ƫ����
	uint16_t num = 0;
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //���
	
	while(*s++) {
		num++;
	}

	if(ascw == 0) {
		ascw = 65535;
	}
	
	while(num)
	{
		num--;
		showbuf[0] = str[strnum];
		if(showbuf[0] > 0x7f) { //�ж��Ƿ�Ϊ����
			showbuf[1] = str[strnum + 1];
			strnum += 2;
			num--;
			fontinfo.hznum++;
		}
		else {
			strnum += 1;
		}
		if(showbuf[0] == '\n') {
			showx = 0;
			showy++;
			
			fontinfo.LBnum++;
			if(maxPix > fontinfo.maxPix) fontinfo.maxPix = maxPix;
			maxPix=0;
			
			continue;
		}
		
		if(showbuf[0] > 0x7f) { //������ʾ
			MenuHZ16x16(target,  x + showx,  y + showy*offset, showbuf);
			showbuf[1] = 0;
			showx += 16;
			
			maxPix+=16;
		}
		else { //Ӣ����ʾ
			MenuShowAscStr(target, x + showx, y + showy*offset + (16 - font_SizeInf[1]), showbuf);
			showx += font_SizeInf[0];
			
			fontinfo.ascnum++;
			maxPix+=font_SizeInf[0];
		}
		if(showx >= ascw) { //��ȳ���
			showx = 0;
			showy++;
			
			fontinfo.LBnum++;
			if(maxPix > fontinfo.maxPix) fontinfo.maxPix = maxPix;
			maxPix=0;
			
		}
	}
	
	fontinfo.ascsize[0] = font_SizeInf[0];
	fontinfo.ascsize[1] = font_SizeInf[1];
	fontinfo.hzsize[0] = 16;
	fontinfo.hzsize[1] = 16;
	if(maxPix > fontinfo.maxPix) fontinfo.maxPix = maxPix;
	
	return &fontinfo;
}

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
static uint8_t PRINTF_BUFF[100];
FontInfoType* m_printf(menu_area *target,int16_t x, int16_t y, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf((char *)PRINTF_BUFF, sizeof(PRINTF_BUFF), (char *)format, args);
	va_end(args);
	
	FontInfoType *FontInfo = StringDeal(PRINTF_BUFF);
	
	if(FontInfo->hznum)
		return CharacterTextC(target, x, y, 0, PRINTF_BUFF);
	else
		return CharacterText(target, x, y, 0, PRINTF_BUFF);
}

static const uint8_t *NumPic = F8X16+2+(16*('0'-' '));
static const uint8_t *NUmPicSize = F8X16;
static uint16_t NumPicZf_x=100, NumPicZf_y=100;

void NumScrollSet(const uint8_t *pic, uint16_t zf_x, uint16_t zf_y)
{
	NumPic = pic+2;
	NUmPicSize = pic;
	NumPicZf_x = zf_x;
	NumPicZf_y = zf_y;
}
/*
	���ܣ����ֹ�����ʾ y���� ��������
	target���˵������ΪNULLû�����壩
	x��y ������ڲ˵�������
	mem�������¼λ�ã����ʼ��Ϊ0
	now_nub����ǰ��ֵ
	want_nub��������ֵ
	speed: �����ٶ�
 */
void NumScrollShow(menu_area *target, int16_t x, int16_t y, uint8_t *mem, uint8_t *now_nub, uint8_t want_nub, uint8_t speed)
{
	if(*now_nub != want_nub)
	{
		*mem+=speed;
		if(*mem > (NUmPicSize[1]-1)*NumPicZf_y/100 )
		{
			*mem=0;
			*now_nub = want_nub;
		}
	}
	ImageScaling(target, NUmPicSize, NumPic+DIVIDEUP(NUmPicSize[0]*NUmPicSize[1])*(*now_nub), x, y-(*mem), NumPicZf_x, NumPicZf_y);
	if(*mem)
	{
		ImageScaling(target, NUmPicSize, NumPic+DIVIDEUP(NUmPicSize[0]*NUmPicSize[1])*want_nub, x, NUmPicSize[1]*NumPicZf_y/100-(*mem), NumPicZf_x, NumPicZf_y);
	}
}


/*
	���ܣ��ַ�������
	
	str���ַ����� \0 ��β��
	
	ret����������Ϣ
*/

FontInfoType* StringDeal(const uint8_t *str)
{
	static FontInfoType fontinfo;	
	
	const uint8_t *s = str;
	
	uint16_t strlen=0;
	uint16_t i;
	uint16_t maxPix=0;
	
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //���
	
	while(*(s++)) //�ַ�������
	{
		strlen++;
	}
	
	for(i=0; i<strlen; i++)
	{
		if(str[i] > 0x7F) //����
		{
			i++;
			fontinfo.hznum++;
			maxPix+=16;
		}
		else
		{
			if(str[i]=='\n')
			{
				fontinfo.LBnum++;
				if(maxPix > fontinfo.maxPix) fontinfo.maxPix = maxPix;
				maxPix=0;
			}
			else
			{
				maxPix+=font_SizeInf[0];
			}
			fontinfo.ascnum++;
		}
	}
	
	
	fontinfo.ascsize[0] = font_SizeInf[0];
	fontinfo.ascsize[1] = font_SizeInf[1];
	fontinfo.hzsize[0] = 16;
	fontinfo.hzsize[1] = 16;
	if(maxPix > fontinfo.maxPix) fontinfo.maxPix = maxPix;
	
	return &fontinfo;
	
}


/*
	���ܣ��Զ�����ָʾ���ߴ�

	FontInfo��������Ϣ���
	target��ָʾ�����
	limitSize���ߴ����ƣ������ÿ���NULL������Ϊ{��խ����� ������}��ĳλ�����ƿ�����0

	ret�������ߴ��޸�ʱ������1�� ���򷵻�0

*/

uint8_t SetIndicatorSize(FontInfoType *FontInfo, menu_area *target, const uint8_t *limitSize)
{
	uint16_t width;
	uint16_t high;
	uint8_t changeFlag=0;
	
	width = FontInfo->maxPix;
	
	switch(FontInfo->hznum)
	{
		case 0:
			high = (FontInfo->LBnum + 1) * FontInfo->ascsize[1];
			break;
		
		default:
			high = (FontInfo->LBnum + 1) * (FontInfo->hzsize[1] > FontInfo->ascsize[1] ? FontInfo->hzsize[1] : FontInfo->ascsize[1]);
			break;
	}
	
	if(limitSize)
	{
		if(limitSize[0]) width = width < limitSize[0] ? limitSize[0] : width;
		if(limitSize[1]) high  = high < limitSize[1] ? limitSize[1] : high;
		if(limitSize[2]) width = width > limitSize[2] ? limitSize[2] : width;
		if(limitSize[3]) high  = high > limitSize[3] ? limitSize[3] : high;
	}
	
	if(target->width!=width || target->high!=high)
	{
		target->width = width;
		target->high = high;
		changeFlag = 1;
	}
	return changeFlag;
}
// ----------------- �� �� ------------------

static uint8_t MenuSerialBuf[MenuSerialBufSzie] = {0}; //���ڻ�����
uint16_t MenuSerialRxNum = 0; //�������������ΪMenuSerialBufSzie
static uint16_t MenuSerialShowPPos[2] = {0, 0}; //���һ����ʾ�ַ�����һ������

static uint16_t SerialstartShow = 0; //��ʾ��ʼλ��
static uint16_t MenuSerialShowP = 0; //��ʾ��ֹλ��
static uint8_t stoprecive = DISABLE;

// ENABLE ֹͣ����
void MenuSerStopRecive(uint8_t enb)
{
	stoprecive = enb;
}
uint8_t SerisStop(void)
{
	return stoprecive;
}

/*
	���ܣ���ʾ�����ı�

target���˵�ָ��
x���ı������Ͻ�x����
y���ı������Ͻ�y����
ascw����ʾ��ȣ�<���ַ�λ��λ>
asch����ʾ�߶ȣ�<���ַ�λ��λ>

ע�⣺δʶ��������ʾΪ�ո񣬲�֧��������ʾ����
*/

void SerialCharacterText(menu_area *target, int16_t x, int16_t y)
{
	uint8_t showbuf[2] = {0, 0};
	uint16_t strnum = SerialstartShow;
	uint8_t showx=0, showy=0;
	uint8_t *str = MenuSerialBuf; //��ʾ������
	uint16_t num = SerialstartShow; 
	
	while(num != MenuSerialShowP)
	{
		if(++num >= MenuSerialBufSzie) {
			num = 0;
		}
		showbuf[0] = str[strnum];
		if(++strnum >= MenuSerialBufSzie) {
			strnum = 0;
		}
		if(showbuf[0] == '\0') showbuf[0] = ' ';
		if(showbuf[0] == '\n') {
			showx = 0;
			if(++showy >= MENUSERIALWINDOWHIGH) {
				break;
			}
			continue;
		}
		MenuShowAscStr(target,  x + showx*font_SizeInf[0],  y + showy*font_SizeInf[1], showbuf);
		
		if(++showx >= MENUSERIALWINDOWWIDTH) {
			showx = 0;
			if(++showy >= MENUSERIALWINDOWHIGH) {
				break;
			}
		}
	}
}
//��մ�����ʾ
void ClearnSerialShowBuf(void)
{
	MenuSerialRxNum = 0;
	MenuSerialShowP = 0;
	SerialstartShow = 0;
	MenuSerialShowPPos[0] = 0;
	MenuSerialShowPPos[1] = 0;
}
/*
	���ܣ���ʼˢ�º�����
	n�����Ƶ�����
*/
static void SerialstartN(uint8_t n)
{
	uint8_t i = 0;

	while(n--) {
		i = MENUSERIALWINDOWWIDTH;
		while(i--) {
			if(MenuSerialBuf[SerialstartShow] == '\n') {
				SerialstartShow++;
				if(SerialstartShow >= MenuSerialBufSzie) {
					SerialstartShow = SerialstartShow - MenuSerialBufSzie;
				}
				break;
			}
			SerialstartShow++;
			if(SerialstartShow >= MenuSerialBufSzie) {
				SerialstartShow = 0;
			}
		}
	}
}
//�������
static void MenuPaddingSerialBuf(uint8_t dat) 
{
	uint8_t flog = 0;
	
	MenuSerialBuf[MenuSerialShowP] = dat;
	if(++MenuSerialRxNum >= MenuSerialBufSzie) {
		MenuSerialRxNum = MenuSerialBufSzie;
	}
	if(++MenuSerialShowP >= MenuSerialBufSzie) { //��ʾ��ֹλ��
		MenuSerialShowP = 0;
	}
	
	//����\n�ַ�
	if(dat == '\n') {
		if(MenuSerialShowPPos[0] == MENUSERIALWINDOWWIDTH) { //ͬʱһ�������
			MenuSerialShowPPos[1]+=2;
		}
		else { //��������
			MenuSerialShowPPos[1]+=1;
		}
		MenuSerialShowPPos[0] = 0;
		
		if(MenuSerialShowPPos[1] >= MENUSERIALWINDOWHIGH) { //��������ʾ�߽�
			SerialstartN(MenuSerialShowPPos[1] - MENUSERIALWINDOWHIGH + 1); //�ƶ���ʼ��ʾ��
			MenuSerialShowPPos[1] = MENUSERIALWINDOWHIGH-1;
		}
		return;
	}
	
	//��\n��������ַ��Ľ���
	if(MenuSerialShowPPos[0] >= MENUSERIALWINDOWWIDTH) {
		if(MenuSerialShowPPos[1] == MENUSERIALWINDOWHIGH-1) {
			if(MenuSerialShowPPos[0] == MENUSERIALWINDOWWIDTH) {
					flog = 1;
			}
		}
		else {
			if(++MenuSerialShowPPos[1] >= MENUSERIALWINDOWHIGH) {
				MenuSerialShowPPos[1] -= 1;
				flog = 1;
			}
		}
		if(flog) {
			flog = 0;
			SerialstartN(1);
		}
		MenuSerialShowPPos[0] = 0;
	}
	MenuSerialShowPPos[0]++;
}

static uint8_t isHexFlag = 0;
uint8_t SerisHex(void)
{
	return isHexFlag;
}
void SerSetHex(uint8_t isHex)
{
	isHexFlag = isHex;
}

void MenuPaddingSerial(uint8_t rx)
{
	if(stoprecive == ENABLE) return;
	if(isHexFlag == 0)
	{
		MenuPaddingSerialBuf(rx);
	}
	else
	{
		static const uint8_t hexascii[]={"0123456789ABCDEF"};

		MenuPaddingSerialBuf(hexascii[rx/16]);
		MenuPaddingSerialBuf(hexascii[rx%16]);
		MenuPaddingSerialBuf(' ');
	}
}

/*
	�����ַ���ƥ��
	@ret 1:����ƥ���ַ� 0��ƥ��ʧ��
*/
uint8_t SerStrMatching(const uint8_t *str)
{
	uint16_t strstart = SerialstartShow;
	uint16_t strend = MenuSerialShowP;
	uint16_t mystrtotal;
	
	uint8_t matnum = 0;
	
	mystrtotal = mystrlen(str);

	if(mystrtotal > MenuSerialRxNum) {
		
		return 0;
	}
	
	for(; strstart != strend; ) {
		if(MenuSerialBuf[strstart] == str[matnum]) {
			matnum++;
		}
		else {
			matnum = 0;
		}
		if(matnum == mystrtotal) {
			return 1;
		}
		
		if(++strstart == MenuSerialBufSzie) {
			strstart = 0;
		}
	}
	return 0;
}




