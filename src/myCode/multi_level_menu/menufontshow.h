#ifndef _menufontshow_h_
#define _menufontshow_h_

#include "menu.h"
#include "menuconfig.h"

typedef struct FONTINFO
{
	uint16_t ascnum; //英文数量
	uint16_t hznum;  //汉字数量
	uint16_t maxPix; //最长的一行像素值
	uint8_t ascsize[2];//英文尺寸
	uint8_t hzsize[2]; //汉字尺寸
	uint8_t LBnum; //换行数量
}FontInfoType;


extern const uint8_t *font_SizeInf; //字体大小

// 设置英文显示字体
void SetFont(const uint8_t *xfont);

//复位默认字体
void RestFont(void);


// -- 单行显示 --

// 功能：在菜单中写Asc字符串，target为NULL在屏幕写
FontInfoType* MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str);
// 汉字 Asc 混合显示，target为NULL在屏幕写
FontInfoType* MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat);

// -- 多行显示 --

// 文本显示 <仅支持英文>   <ascw 字符个数！！>
FontInfoType* CharacterText(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);
// 文本显示 <中英文混合显示> 行间隔以最大的字体高度执行！！ <ascw 像素>
FontInfoType* CharacterTextC(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);


// 功能：在菜单中写Asc数字，target为NULL在屏幕写
FontInfoType * MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num);


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
FontInfoType* m_printf(menu_area *target, int16_t x, int16_t y, const char *format, ...);

/*
	功能：数字滚动显示 y方向 单个数字
	target：菜单句柄（为NULL没有意义）
	x，y ：相对于菜单的坐标
	mem：用与记录位置，需初始化为0
	now_nub：当前的值
	want_nub：期望的值
	speed: 滚动速度
 */
void NumScrollShow(menu_area *target, int16_t x, int16_t y, uint8_t *mem, uint8_t *now_nub, uint8_t want_nub, uint8_t speed);
void NumScrollSet(const uint8_t *pic, uint16_t zf_x, uint16_t zf_y);

/*
	功能：字符串处理
	
	str：字符串（ \0 结尾）
	
	ret：处理后的信息
*/

FontInfoType* StringDeal(const uint8_t *str);

/*
	功能：自动调整指示器尺寸

	FontInfo：字体信息句柄
	target：指示器句柄
	mod： 0：使用的函数与中文无关  1：函数关于中文
	limitSize：尺寸限制，不设置可置NULL。内容为{最窄，最矮， 最宽，最高}，某位不限制可以置0
	ret：发生尺寸修改时，返回1， 无则返回0
*/
uint8_t SetIndicatorSize(FontInfoType *FontInfo, menu_area *target, const uint8_t *limitSize);

// -------------- 串 口 --------------


#define MENUSERIALWINDOWWIDTH   21 //窗口宽度，字节为单位 max:255
#define MENUSERIALWINDOWHIGH   5 //窗口高度，字节为单位   max:255

#define MenuSerialBufSzie  (MENUSERIALWINDOWWIDTH * MENUSERIALWINDOWHIGH + 1) //串口缓冲区字节大小


extern uint16_t MenuSerialRxNum; //接收数量，最大为MenuSerialBufSzie


//串口填充
void MenuPaddingSerial(uint8_t rx);
// 串口文本显示<仅支持英文>
void SerialCharacterText(menu_area *target, int16_t x, int16_t y);
//清空串口显示
void ClearnSerialShowBuf(void);

void MenuSerStopRecive(uint8_t enb);
uint8_t SerisStop(void);

void SerSetHex(uint8_t isHex);
uint8_t SerisHex(void);
/*
	串口字符串匹配
	@ret 1:含由匹配字符 0：匹配失败
*/
uint8_t SerStrMatching(const uint8_t *str);
	

#endif



