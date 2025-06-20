#include <stdarg.h>
#include <stdio.h>

#include "menufontshow.h"
#include "Graphicalfunctions.h"
#include "oledfont.h"

static uint8_t fontchangeflag=0;
static const uint8_t *font = F8X16+2;
const uint8_t *font_SizeInf = F8X16;

//设置英文显示字体
void SetFont(const uint8_t *xfont)
{
	fontchangeflag = 1;
	font = xfont+2;
	font_SizeInf = xfont;
}

//复位默认字体
void RestFont(void)
{
	if(fontchangeflag)
	{
		font = F8X16+2;
		font_SizeInf = F8X16;
	}
}

/*
	功能：在菜单中写Asc字符，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	asc：一个ASCII字符
*/
static void MenuShowAsc(menu_area *target,int16_t x, int16_t y, uint8_t asc)
{
	uint8_t c=0;
	uint8_t h,w;
	bool w_b=0;
	uint8_t wight = font_SizeInf[0];
	uint8_t high  = font_SizeInf[1];
	
	if(asc > '~' || asc < ' ') asc = ' '; //不在显示范围
	c=asc-' ';   // ' '=32,ASCII码表
	
	
	if(target == NULL) 
	{
		DrawPicture(NULL,font_SizeInf, font + c*wight*DIVIDEUP(high), x, y);
	}
	else
	{
		if( (x+font_SizeInf[0]-1)<0 || (y+font_SizeInf[1]-1)<0 || x>=target->width || y>=target->high) //不在显示区域
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
			
			for(h=start_y; h<end_y; h++)//字高
			{
				for(w=start_x; w<end_x; w++) //字宽
				{
					w_b = ( font[c*wight*DIVIDEUP(high) + w+wight*(h/8)] >> (h%8) ) & 1;
					if(w_b) {
						MenuSetPoint(target, w+x, h+y, 1);
					}
					else { //覆盖显示
						if(GRAPHICSSHOWMANNER == GraphicsCover) {
							MenuSetPoint(target, w+x, h+y, 0);
						}
					}
				}
			}
		}
	}
}
//m^n函数
static uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

/*
	功能：在菜单中写Asc数字，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	len：数字长度 0自动
	num：数字
*/
FontInfoType * MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num)
{     
	static FontInfoType fontinfo;
		
	uint8_t t,temp;
	uint8_t enshow=0;	
	uint32_t nump = num;
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //清空
	
	if(len == 0) { //len=0 , 自动计算长度
		if(nump == 0) {
			len = 1;
		}
		while(nump) {
			len++;
			nump/=10;
		}
	}
	
	fontinfo.ascnum = len; //信息更新
	
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
	功能：在菜单中写Asc字符串，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	str：一个ASCII字符串
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
	功能：英文显示文本

target：菜单指针
x：文本的左上角x坐标
y：文本的左上角y坐标
ascw：显示宽度 《字符个数！！》
str：显示的数据串
注意：数据\n为回车，其它未识别数据显示为空格，不支持中文显示！！
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
			
			fontinfo.LBnum++;//字符数据更新
			if(maxPix >fontinfo.maxPix ) fontinfo.maxPix = maxPix;
			maxPix = 0;
			
			continue;
		}
		MenuShowAsc(target,x + showx*font_SizeInf[0],  y + showy*font_SizeInf[1], showbuf);

		if(++showx >= ascw) {
			showx = 0;
			showy++;
			
			fontinfo.LBnum++;//字符数据更新
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




// ---------------- 汉 字 显 示------------------------


/*
	功能：测量以\0为结尾的字符串长度
*/
static unsigned int mystrlen(const uint8_t *str)
{
	unsigned int i=0;
	for(i=0; str[i] != '\0'; i++);
	return i;
}

///*
//	功能：测量以\0为结尾的 汉字与字符 的字符串长度
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
//	功能：从已知字符串中找想要的字符的位置
//	formfind：目标字符串
//	find：想要寻找的字符所在的字符串
//	num：find中第num的字符
//	返回值：寻找字符在formfind中的位置
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
//// 汉字显示，target为NULL在屏幕写
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
//			continue; // 没有找到
//		}

//		for(j=0; j<16; j++) //高度
//		{
//			for(i=0; i<16; i++) //宽度
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
	功能：单个汉字在字符串中滤去ascii的位置
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
// 单个汉字显示，target为NULL在屏幕写
static void MenuHZ16x16(menu_area *target, int16_t x, int16_t y, uint8_t *s_dat)
{
	unsigned int j0=0;
	unsigned char i,j;
	
	const uint8_t psize[] = {16, 16};
	
	
	j0=HzInStr(HZK16x16index, s_dat);
	if(j0 == 0) {
		return; // 没有找到
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
			
			for(j=start_y; j<end_y; j++) //高度
			{
				for(i=start_x; i<end_x; i++) //宽度
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

// 汉字 Asc 混合显示，target为NULL在屏幕写
FontInfoType* MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat)
{
	static FontInfoType fontinfo;	
	
	unsigned int s_datlen = mystrlen(s_dat);
	unsigned int i=0;
	unsigned char HZShowBuf[] = {0,0,0}; // 必须三位，以0结尾
	uint8_t offset = font_SizeInf[1] < 16 ? 0 : font_SizeInf[1] - 16; //上下偏移量
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //清空
	
	for(i=0; i<s_datlen; )
	{
		if(s_dat[i] > 127) //汉字
		{
			HZShowBuf[0]=s_dat[i];
			HZShowBuf[1]=s_dat[i+1];
			MenuHZ16x16(target, x, y + offset, HZShowBuf);
			x+=16; // 坐标右移 由汉字宽度决定
			i+=2;  // 字符串位置标志右移，汉字固定为2
			fontinfo.hznum++;
		}
		else   //Asc 
		{
			MenuShowAsc(target, x, y + offset + (16 - font_SizeInf[1]), s_dat[i]);
			x+=font_SizeInf[0]; //坐标右移 由Asc宽度决定
			i++;  //字符串位置标志右移，Asc固定为1
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
	功能：中文显示文本

target：菜单指针
x：文本的左上角x坐标
y：文本的左上角y坐标
ascw：显示宽度 <像素>
str：显示的数据串
注意：数据\n为回车，其它未识别数据显示为空格，行间隔以最大的字体高度执行！！
*/
FontInfoType* CharacterTextC(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str)
{
	static FontInfoType fontinfo;	
	uint16_t maxPix = 0;
	
	uint8_t showbuf[3] = {0, 0, 0};
	uint16_t strnum = 0;
	uint16_t showx=0, showy=0;
	const uint8_t *s = str;
	uint8_t offset = font_SizeInf[1] < 16 ? 16 : font_SizeInf[1]; //文本上下偏移量
	uint16_t num = 0;
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //清空
	
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
		if(showbuf[0] > 0x7f) { //判断是否为中文
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
		
		if(showbuf[0] > 0x7f) { //中文显示
			MenuHZ16x16(target,  x + showx,  y + showy*offset, showbuf);
			showbuf[1] = 0;
			showx += 16;
			
			maxPix+=16;
		}
		else { //英文显示
			MenuShowAscStr(target, x + showx, y + showy*offset + (16 - font_SizeInf[1]), showbuf);
			showx += font_SizeInf[0];
			
			fontinfo.ascnum++;
			maxPix+=font_SizeInf[0];
		}
		if(showx >= ascw) { //宽度超出
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
	功能：如使用printf向屏幕打印字符
	target：目标菜单
	mod：0 内部调用CharacterText，不支持中文，但输出紧凑
			 1 内部调用CharacterTextC，支持中文，但行间可能有空隙
	x,y: 坐标
	formate与..: 与printf相同
	ret：输出的字符个数
	注意：内部申请固定内存100字节，不要输出太长字符串
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
	功能：数字滚动显示 y方向 单个数字
	target：菜单句柄（为NULL没有意义）
	x，y ：相对于菜单的坐标
	mem：用与记录位置，需初始化为0
	now_nub：当前的值
	want_nub：期望的值
	speed: 滚动速度
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
	功能：字符串处理
	
	str：字符串（ \0 结尾）
	
	ret：处理后的信息
*/

FontInfoType* StringDeal(const uint8_t *str)
{
	static FontInfoType fontinfo;	
	
	const uint8_t *s = str;
	
	uint16_t strlen=0;
	uint16_t i;
	uint16_t maxPix=0;
	
	
	ClearnMemory(&fontinfo, sizeof(FontInfoType)); //清空
	
	while(*(s++)) //字符串长度
	{
		strlen++;
	}
	
	for(i=0; i<strlen; i++)
	{
		if(str[i] > 0x7F) //中文
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
	功能：自动调整指示器尺寸

	FontInfo：字体信息句柄
	target：指示器句柄
	limitSize：尺寸限制，不设置可置NULL。内容为{最窄，最矮， 最宽，最高}，某位不限制可以置0

	ret：发生尺寸修改时，返回1， 无则返回0

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
// ----------------- 串 口 ------------------

static uint8_t MenuSerialBuf[MenuSerialBufSzie] = {0}; //串口缓冲区
uint16_t MenuSerialRxNum = 0; //接收数量，最大为MenuSerialBufSzie
static uint16_t MenuSerialShowPPos[2] = {0, 0}; //最后一个显示字符的下一个坐标

static uint16_t SerialstartShow = 0; //显示起始位置
static uint16_t MenuSerialShowP = 0; //显示截止位置
static uint8_t stoprecive = DISABLE;

// ENABLE 停止接收
void MenuSerStopRecive(uint8_t enb)
{
	stoprecive = enb;
}
uint8_t SerisStop(void)
{
	return stoprecive;
}

/*
	功能：显示串口文本

target：菜单指针
x：文本的左上角x坐标
y：文本的左上角y坐标
ascw：显示宽度，<以字符位单位>
asch：显示高度，<以字符位单位>

注意：未识别数据显示为空格，不支持中文显示！！
*/

void SerialCharacterText(menu_area *target, int16_t x, int16_t y)
{
	uint8_t showbuf[2] = {0, 0};
	uint16_t strnum = SerialstartShow;
	uint8_t showx=0, showy=0;
	uint8_t *str = MenuSerialBuf; //显示缓冲区
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
//清空串口显示
void ClearnSerialShowBuf(void)
{
	MenuSerialRxNum = 0;
	MenuSerialShowP = 0;
	SerialstartShow = 0;
	MenuSerialShowPPos[0] = 0;
	MenuSerialShowPPos[1] = 0;
}
/*
	功能：起始刷新后移行
	n：后移的行数
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
//串口填充
static void MenuPaddingSerialBuf(uint8_t dat) 
{
	uint8_t flog = 0;
	
	MenuSerialBuf[MenuSerialShowP] = dat;
	if(++MenuSerialRxNum >= MenuSerialBufSzie) {
		MenuSerialRxNum = MenuSerialBufSzie;
	}
	if(++MenuSerialShowP >= MenuSerialBufSzie) { //显示截止位置
		MenuSerialShowP = 0;
	}
	
	//接收\n字符
	if(dat == '\n') {
		if(MenuSerialShowPPos[0] == MENUSERIALWINDOWWIDTH) { //同时一行填充满
			MenuSerialShowPPos[1]+=2;
		}
		else { //正常行移
			MenuSerialShowPPos[1]+=1;
		}
		MenuSerialShowPPos[0] = 0;
		
		if(MenuSerialShowPPos[1] >= MENUSERIALWINDOWHIGH) { //超出行显示边界
			SerialstartN(MenuSerialShowPPos[1] - MENUSERIALWINDOWHIGH + 1); //移动起始显示行
			MenuSerialShowPPos[1] = MENUSERIALWINDOWHIGH-1;
		}
		return;
	}
	
	//除\n外的其它字符的接收
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
	串口字符串匹配
	@ret 1:含由匹配字符 0：匹配失败
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




