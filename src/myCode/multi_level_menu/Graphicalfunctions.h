#ifndef _Graphicalfunctions_h_
#define _Graphicalfunctions_h_

#include "menu.h"
#include "menu_tool.h" //工具

// 画点
void write_point(int16_t x, int16_t y, uint8_t w_d);
// 读点
unsigned char read_point(int16_t x, int16_t y);

//在8x128的数组里画线
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

//空心圆
void DrawCircle(int16_t x0,int16_t y0,uint8_t r);
//实心圆
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r);

//空心矩形
void DrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h);
//实心矩形
void DrawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h);

//空心圆角矩形
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
//实心圆角矩形
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);

//多边形变换
void PolygonTransformation(int16_t x0,int16_t y0,uint16_t r, uint16_t n);

//旋转目标点
void RotateXY(int *xy, int centerX, int centerY, int x, int y, int Angle, char direct);

//线立方体 cx、cy：中心坐标  w、h宽高  rot：以0.1为单位递增即可（弧度制）
void Linecube(int cx, int cy, int w, int h, double rot);

//线正方体 centerX、centerY 中心坐标 ， size：边长 ， rotx roty rotz：绕xyz轴的角度（弧度制）
void DrawCube(int centerX, int centerY, int size, float rotX, float rotY, float rotZ);





//------------------  折 线 图 -------------------------



typedef struct LineChartMap
{
	const uint8_t width; //宽度 0-128
	const uint8_t high;  //高度 0-128
	const uint16_t max;  //取值范围
	uint8_t Startp; //起始显示位置 
	uint8_t Endp;   //结束显示位置
	uint8_t RxNum;
	uint8_t dat[128];
}TypLineChartMap;
//定义实例
//TypLineChartMap p = {width, high, max};

//折线图内存清零 <初始化内存>
void ClearnLineChartMapDat(TypLineChartMap *t);
//向折线图中填充数据
void AddDatToLineChartMap(TypLineChartMap *t, uint16_t d);
//功能：折线图绘制 @ret：最后一个数据在折线图中的显示坐标
int16_t *LineChart(TypLineChartMap *t, int16_t x, int16_t y);






// ---------------- 图 片 -------------------


/*
	功能：图片显示
	取模格式：阴码 列行式 逆向

	注意：若psize为NULL，图片大小应定义在p中，否则大小将按psize设置（真实大小，该函数不会缩放图片）
*/
void DrawPicture(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y);
/*
	功能：图像缩放显示
	psize：原始尺寸信息，为NULL则使用p中头两位
	x，y：相对于屏幕的坐标
	zf_x，zf_y:x、y方向的缩放倍数。100为原尺寸
*/
void ImageScaling(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y, uint16_t zf_x, uint16_t zf_y);


// -------------------- 进 度 条 -----------------------

enum PROGRESSBAR_ATTR
{
	hideframe     = 0x01,  //隐藏边框
	hideindicator = 0x02,  //隐藏指示器
};

typedef struct  PROGRESSBAR
{
	uint16_t BarVal;    //存储变量
	uint16_t pixVal;    //进度条像素信息
	uint16_t maxVal;    //允许最大值
	uint8_t width;     //宽度
	uint8_t high;       //高度
	uint8_t R;          //圆角
	uint8_t attribute;  //属性
	bool animation;     //动画
	uint8_t direction;  //方向 0-3
	TanLevPIDTypedef pidhandle; //pid
}ProgressBarTypedef;

/*
	功能：进度条初始化
	barobj : 句柄
	direction : 0-3， 显示方向，顺时针旋转
	weight : 宽度，正视屏幕
	high   : 高度，正视屏幕
	maxVal : 进度条满时最大值
	
*/ 
ProgressBarTypedef * ProgressBarInit(ProgressBarTypedef *barobj, uint8_t direction, uint8_t width, uint8_t high, uint16_t maxVal);
/*
	功能：进度条显示
	barobj：句柄
	x：x坐标
	y：y坐标
	val：输入值，与maxVal比较计算填充比例
	
	ret: 指示器末端中心坐标

	注意：默认无动效，需自主使能 animation ，并定时调用
*/
int16_t * ProgressBar(ProgressBarTypedef *barobj, int16_t x, int16_t y, uint16_t Val);


//功能：获取bar的值
uint16_t getBarVal(ProgressBarTypedef *barobj);

//进度条属性设置
void SetProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute);

//进度条属性复位
void ResProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute);






// ----------------- 模 糊 --------------------- 


/*
	功能：oled界面模糊
	n: 0-4  0不模糊 >=4类似清屏
*/
void InterfaceBlurry(uint8_t n);

/*
	功能：菜单进入、离开虚化
	state: 0 菜单截停  1 菜单虚化
	
	ret：1 开始虚化（处于当前菜单）  0 开始实化（处于切换后的菜单）

	注意： ...
				触发虚化时 输入响应 被禁止，内部主动改变指针时，请先判断是否能 输入响应，
				再改变菜单指针
				state=0 时，函数应放在   MenuAlwaysRun_PH 函数最开始处，
				state=1 时，函数尽量放在 MenuAlwaysRun_PL 函数结束处
*/
uint8_t MenuDynamicBlurry(uint8_t state);

#endif








