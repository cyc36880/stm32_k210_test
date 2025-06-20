#include "Graphicalfunctions.h"
#include "menu_tool.h"

#include "math.h"

// ======================= 图 形 化 函 数 ========================


static unsigned char ReadPoint(int16_t x, int16_t y)
{
	int px = x, py = (int)(y/8);
	unsigned char dat=0;
	
	if(x<0 || y<0) return 0;
	if(x>= SCREENWIDTH-1 || y>=SCREENHIGH) return 0; // x 的 -1即放弃的最右侧一列缓冲区，可自行修改
	
	dat = (DisplayBuff[px + py*SCREENWIDTH]>>(y%8)) & 0x01;
	
	return dat;
}

static void WritePoint(int16_t x, int16_t y, uint8_t w_d) 
{
	int px = x, py = (int)(y/8);
	
	if(x>=SCREENWIDTH-1|| y >= SCREENHIGH) return; // x 的 -1即放弃的最右侧一列缓冲区，可自行修改
	if(x<0 || y<0) return;
	
	if(w_d)
		DisplayBuff[px + py*SCREENWIDTH] |= (0x01 << (y % 8));
	else
		DisplayBuff[px + py*SCREENWIDTH] &= ~(0x01 << (y % 8));
}

// 读点
unsigned char read_point(int16_t x, int16_t y)
{
	unsigned char point = 0;
	point = ReadPoint(x, y);
	return point;
}

// 画点
void write_point(int16_t x, int16_t y, uint8_t w_d) 
{
	switch(GRAPHICSSHOWMANNER)
	{
		case GraphicsNormal:   
			WritePoint(x,  y,  w_d);
			break;
		case GraphicsRollColor: //反转显示
			WritePoint(x,  y,  !read_point(x, y));
			break;
		case GraphicsColless:
			WritePoint(x, y, 0);
			break;
		default:
			WritePoint(x,  y,  w_d);
			break;
	}
}
void menu_write_point(menu_area *target, int16_t x, int16_t y,  uint8_t w_d)
{
	if(target == NULL)
	{
		write_point(x, y, w_d);
	}
	else
	{
		if(x<0 || y<0) return;
		if(x>=target->width || y>=target->high) return;
		write_point(target->x + x, target->y+y, w_d);
	}
}

//横向快速画线
static void FastLineT(int16_t x, int16_t y, uint16_t w)
{
	while(w) {
		write_point(x+w-1, y, 1);
		w--;
	}
}
//纵向快速画线
static void FastLineL(int16_t x, int16_t y, uint16_t h)
{
	while(h){
		write_point(x, y+h-1, 1);
		h--;
	}
}
//空心矩形
void DrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	if(h==0 || w==0) return;
	FastLineT(x, y, w);
	FastLineT(x, y+h-1, w);
	if(h<3) return;
	FastLineL(x, y+1, h-2);
	FastLineL(x+w-1, y+1, h-2);
}
//实心矩形
void DrawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	while(h) {
		FastLineT(x, y+h-1, w);
		h--;
	}
}

// 画线
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{
		write_point(uRow,uCol, 1);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
// 空心圆
void DrawCircle(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		write_point(x0+a,y0-b,1);             //5
 		write_point(x0+b,y0-a,1);             //0           
		write_point(x0+b,y0+a,1);             //4               
		write_point(x0+a,y0+b,1);             //6 
		write_point(x0-a,y0+b,1);             //1       
 		write_point(x0-b,y0+a,1);             
		write_point(x0-a,y0-b,1);             //2             
  		write_point(x0-b,y0-a,1);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 
//实心圆
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		DrawLine(x0+a,y0-b,x0+a,y0+b);
		DrawLine(x0+b,y0-a,x0+b,y0+a);
		DrawLine(x0-a,y0-b,x0-a,y0+b);
		DrawLine(x0-b,y0-a,x0-b,y0+a); 	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
	}
}

// ----------------空 心 圆 角 矩 形------------
#define SWAP(x, y)       \
	    (y) = (x) + (y); \
	    (x) = (y) - (x); \
	    (y) = (y) - (x);
#define MAX(x,y)  		((x)>(y) ? (x):(y))
#define MIN(x,y)  		((x)<(y) ? (x):(y))

void DrawFastHLine(int x, int y, unsigned char w)
{
  int end = x+w;
	int a;
  for ( a = MAX(0,x); a < end; a++)
  {
    write_point(a,y,1);
  }
}

void DrawFastVLine(int x, int y, unsigned char h)
{
  	int end = y+h;
	int a;
  for (a = MAX(0,y); a < end; a++)
  {
    write_point(x,a,1);
  }
}
//实心圆角矩形
void DrawCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername)
{
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;
  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x4)
    {
      write_point(x0 + x, y0 + y,1);
      if(x<y) write_point(x0 + y, y0 + x,1);
    }
    if (cornername & 0x2)
    {
      write_point(x0 + x, y0 - y,1);
      if(x<y) write_point(x0 + y, y0 - x,1);
    }
    if (cornername & 0x8)
    {
      write_point(x0 - y, y0 + x,1);
      if(x<y) write_point(x0 - x, y0 + y,1);
    }
    if (cornername & 0x1)
    {
      write_point(x0 - y, y0 - x,1);
      if(x<y) write_point(x0 - x, y0 - y,1);
    }
  }
}
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r)
{
  // smarter version
  DrawFastHLine(x+r, y, w-2*r); // Top
  DrawFastHLine(x+r, y+h-1, w-2*r); // Bottom
  DrawFastVLine(x, y+r, h-2*r); // Left
  DrawFastVLine(x+w-1, y+r, h-2*r); // Right
  // draw four corners
  DrawCircleHelper(x+r, y+r, r, 1);
  DrawCircleHelper(x+w-r-1, y+r, r, 2);
  DrawCircleHelper(x+w-r-1, y+h-r-1, r, 4);
  DrawCircleHelper(x+r, y+h-r-1, r, 8);
}

void DrawFillCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername, int delta)
{
  // used to do circles and roundrects!
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;
  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1)
    {
      DrawFastVLine(x0+x, y0-y, 2*y+1+delta);
      if(x<y) DrawFastVLine(x0+y, y0-x, 2*x+1+delta);
    }

    if (cornername & 0x2)
    {
      DrawFastVLine(x0-x, y0-y, 2*y+1+delta);
      if(x<y) DrawFastVLine(x0-y, y0-x, 2*x+1+delta);
    }
  }
}

void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r)
{
	if(w==0 || h==0) return ;
	
  DrawFillRect(x+r, y, w-2*r, h);

  // draw four corners
  DrawFillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1);
  DrawFillCircleHelper(x+r, y+r, r, 2, h-2*r-1);
}



//多边形变换
void PolygonTransformation(int16_t x0,int16_t y0,uint16_t r, uint16_t n)
{
	unsigned char i =0,j;
	int x[30],y[30];
		
	for(i=0;i<n;i++)
	{
		x[i]=r*cos(2*3.1415926*i/n)+x0;
		y[i]=r*sin(2*3.1415926*i/n)+y0;
	}
	for(i=0;i<=n-2;i++)
	{
		for(j=i+1;j<=n-1;j++)
			DrawLine(x[i], y[i], x[j], y[j]);
	}
}

#define RADIAN(angle)  ((angle==0)?0:(3.1415926535*angle/180)) // ----------

/*
	centerX : 中心点x坐标
	centerY ：中心点y坐标
	x       ：目标点x坐标
	y       ：目标点y坐标
	angle   ：旋转角度
	direct  ：正反转 1顺时针 0逆时针 
	返回值  ：unsigned int 指针 
*/
void RotateXY(int *xy, int centerX, int centerY,int x, int y,int Angle, char direct) 
{
	double angle = RADIAN(Angle);
	int temp=(y-centerY)*(y-centerY)+(x-centerX)*(x-centerX);
	double r=sqrt(temp);
	double a0=atan2(y-centerY,x-centerX);
	double d=0;
	
	if(Angle==0)
	{
		xy[0] = x;
		xy[1] = y;
		return;
	}
	if(direct)
	{
		d = centerX+r*cos(a0+angle);
		if(((unsigned int)(d*10) )% 10 > 5) xy[0]= (unsigned int)d + 1;
		else                                xy[0]= (unsigned int)d;
		
		d = centerY+r*sin(a0+angle);
		if(((unsigned int)(d*10)) % 10 > 5) xy[1]= (unsigned int)d + 1;
		else                                xy[1]= (unsigned int)d;
	}
	else
	{
		d = centerX+r*cos(a0-angle);
		if(((unsigned int)(d*10) )% 10 > 5) xy[0]= (unsigned int)d + 1;
		else                                xy[0]= (unsigned int)d;
		 
		d = centerY+r*sin(a0-angle);
		if(((unsigned int)(d*10) )% 10 > 5) xy[1]= (unsigned int)d + 1;
		else                                xy[1]= (unsigned int)d;
	}
}





//--------  折 线 图 ------------




//折线图内存清零
void ClearnLineChartMapDat(TypLineChartMap *t)
{
	t->Startp = 0;
	t->Endp = 0;
	t->RxNum = 0;
	ClearnMemory(t->dat, sizeof(t->dat));
}


/* 
	功能：向折线图中填充数据
	t：折线图指针
	d：要填充的数据

*/
void AddDatToLineChartMap(TypLineChartMap *t, uint16_t d)
{
	uint8_t width = t->width<128?t->width:128; //宽度限制
	uint8_t high = t->high<128?t->high:128; //高度限制
	
	if(++t->RxNum > 129) { //接收数量
		t->RxNum = 129;
	}
	if(d > t->max ) d = t->max; //限幅
	d = (uint32_t) high * d / t->max ;
	
	t->dat[t->Endp] = d; //填充数据
	
	if(t->RxNum > width) { 
		if(++t->Startp >= 128) { // 移动起始位置
			t->Startp = 0;
		}
	}
	if(++t->Endp >= 128) {
		t->Endp = 0;
	}
}

/*
	功能：折线图绘制
	t：折线图指针
	x、y：显示的左上角坐标
	@ret：最后一个数据在折线图中的显示坐标
*/
int16_t *LineChart(TypLineChartMap *t, int16_t x, int16_t y) 
{
	static int16_t xy[2] = {0, 0};
	int16_t posy = 0;   //起始位置的相对y坐标
	int16_t posy_2 = 0; //结束位置的相对y坐标
	uint8_t Startp = 0; //数组起始显示位置
	uint8_t Startp_1 = 0;//数组结束显示位置
	
	uint8_t offset=0; //沿x轴的偏移
	
	xy[0] = 0; //返回坐标的默认值
	xy[1] = 0;
	
	if(t->RxNum == 0) { //无接收
		
	}
	else if(t->RxNum == 1) { //接收数1，仅显示点
		
		posy = y + t->high - t->dat[0];
		write_point(x, posy, 1);
		
		xy[0] = x;
		xy[1] = posy;
	}
	else if(t->RxNum == 2) { //接收数2，仅显示一条线
		posy = y + t->high - t->dat[0];   //y轴增量反转
		posy_2 = y + t->high - t->dat[1];
		DrawLine(x, posy, x+1, posy_2);
		
		xy[0] = x + 1;
		xy[1] = posy_2;
	}
	else { //接收多个数，循环显示连续线条
		Startp = t->Startp;
		Startp_1 = Startp + 1 >= 128?0:Startp + 1;
		
		while(Startp_1 != t->Endp) {
			posy = y + t->high - t->dat[Startp]; //y轴增量反转
			posy_2 = y + t->high - t->dat[Startp_1];
			
			DrawLine(x+offset, posy, x+offset+1, posy_2);
			
			Startp = Startp + 1 >= 128?0:Startp + 1;
			Startp_1 = Startp + 1 >= 128?0:Startp + 1;
			
			offset++;
		}
		xy[0] = x+offset-1;
		xy[1] = posy_2;
	}
	return xy;
}



// ---------------- 图 片 -------------------

/*
	功能：图片显示

	size：图片尺寸 max 255 X 255
	p：图片指针
	x、y：坐标

	注意：取模格式> 阴码 列行式 逆向
*/
static void PictureShow(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y)
{
	uint8_t h,w;
	bool w_b=0;
	uint8_t wight;
	uint8_t high;
	
	if(psize == NULL) {
		wight = p[0];
		high  = p[1];
		p += 2;
	}
	else {
		wight = psize[0];
		high  = psize[1];
	}
	for(h=0; h<high; h++)//字高
	{
		for(w=0; w<wight; w++) //字宽
		{
			w_b = ( p[ h/8*wight + w] >> (h%8) ) & 1;
			if(w_b) {
				if(target == NULL) write_point(w+x, h+y, 1);
				else               MenuSetPoint(target, w+x, h+y, 1);
			}
			else {
				if(GRAPHICSSHOWMANNER == GraphicsCover) {
					if(target == NULL) write_point(w+x, h+y, 0);
					else               MenuSetPoint(target, w+x, h+y, 0);
				}
			}
		}
	}
}


/*
	图片显示（快速绘制）
	除无法在菜单中限制显示，其余与上一样
*/
static void FastDrawPic(const uint8_t *pic, int16_t x, int16_t y, uint16_t width, uint16_t high)
{	
	int16_t endx = x+width; //截止显示右下角坐标
	int16_t endy = y+high;
	int16_t beginx;
	uint8_t offset = y<0 ? (8+y%8)%8 : y%8; //偏移量
	
	uint16_t i=0;
	
	if(endx<0 || endy<0) return; //不在屏幕显示范围，跳出
	if(x>=SCREENWIDTH || y>=SCREENHIGH) return;
	
	if(endy > SCREENHIGH-1) endy=SCREENHIGH-1; //滤去y向 向下未显示区域
	if(y<=-8) { //滤去y向 向上未显示区域
		i+=width*( -(y/8) );
		y%=8;
	}
	
	for( ; y<endy; y+=8)
	{
		for(beginx = x; beginx<endx; beginx++, i++)
		{
			if(beginx<0) { //滤去x向 向左未显示区域
				i +=-beginx;
				beginx = 0;
			}
			if(beginx >= SCREENWIDTH-1) { //滤去x向 向右未显示区域
				i += endx - beginx;
				break;
			}
			
			switch(GRAPHICSSHOWMANNER)
			{
				case GraphicsRollColor: //反色
					if(y>=0) 
					DisplayBuff[beginx+((y>>3) )*SCREENWIDTH] ^= pic[i]<<offset;
				
					if(y+8 <= SCREENHIGH-1 && offset) {
						if(y<0)
							DisplayBuff[beginx] ^= pic[i]>>(8-offset);
						else 
							DisplayBuff[beginx+((y>>3)+1 )*SCREENWIDTH] ^= pic[i]>>(8-offset);
					}
					break;
					
				case GraphicsCover: //覆盖
					
					if(y>=0) {
						DisplayBuff[beginx+((y>>3) )*SCREENWIDTH] &= 0xff>>(8-offset);
						DisplayBuff[beginx+((y>>3) )*SCREENWIDTH] |= pic[i]<<offset;
					}
				
					if(y+8 <= SCREENHIGH-1 && offset) {
						if(y<0) {
							DisplayBuff[beginx] &= 0xff<<offset;
							DisplayBuff[beginx] |= pic[i]>>(8-offset);
						}
						else {
							DisplayBuff[beginx+((y>>3)+1 )*SCREENWIDTH] &= 0xff<<offset;
							DisplayBuff[beginx+((y>>3)+1 )*SCREENWIDTH] |= pic[i]>>(8-offset);
						}
					}
					break;
					
				default:
					if(y>=0) 
					DisplayBuff[beginx+((y>>3) )*SCREENWIDTH] |= pic[i]<<offset;
				
					if(y+8 <= SCREENHIGH-1 && offset) {
						if(y<0)
							DisplayBuff[beginx] |= pic[i]>>(8-offset);
						else 
							DisplayBuff[beginx+((y>>3)+1 )*SCREENWIDTH] |= pic[i]>>(8-offset);
					}
			}
		}
	}
}

/*
	图片显示（快速绘制）
	（为与其它函数匹配）除无法在菜单中限制显示，其余与上一样
*/
static void FastDrawPic2(const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y)
{
	uint8_t wight;
	uint8_t high;
	
	if(psize == NULL) {
		wight = p[0];
		high  = p[1];
		p += 2;
	}
	else {
		wight = psize[0];
		high  = psize[1];
	}
	
	FastDrawPic(p, x, y, wight, high);
}
#include "menufontshow.h" //字符函数
/*
	功能：图片显示
	取模格式：阴码 列行式 逆向

	注意：若psize为NULL，图片大小应定义在p中，否则大小将按psize设置（真实大小，该函数不会缩放图片）
*/
void DrawPicture(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y)
{
	if(target == NULL)
	{
		FastDrawPic2(psize, p, x, y);
	}
	else
	{
		PictureShow(target, psize, p, x, y);
	}
}

/*
	功能：图像缩放显示
	psize：原始尺寸信息，为NULL则使用p中头两位
	x，y：相对于屏幕的坐标
	zf_x，zf_y:x、y方向的缩放倍数。100为原尺寸
*/
void ImageScaling(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y, uint16_t zf_x, uint16_t zf_y)
{
	const uint16_t gain = 500; //增益（越大，变化越细腻 下面会乘100，积不要大于65535）
	
	uint16_t i, j, px, py; //遍历图像
	int16_t show_x, show_y;
	uint16_t picsize_w, picsize_h; //图像的宽高
	uint16_t step_x, step_y; //步长 

	if(p == NULL) return;
	if(zf_x==0 || zf_y==0) return; //0缩放不显示
	else if(zf_x==100 && zf_y==100) //原始图像显示
	{
		DrawPicture(target, psize, p, x, y);
		return;
	}
	else
	{
		step_x = 100*gain/zf_x;//步长 
		step_y = 100*gain/zf_y; 
	}
	if(psize == NULL) //确定图像的宽高
	{
		picsize_w = p[0];
		picsize_h = p[1];
		p = p+2;
	}
	else
	{
		picsize_w = psize[0];
		picsize_h = psize[1];
	}

	if(x>=SCREENWIDTH || y>=SCREENHIGH) //判断是否在屏幕的显示范围
		return;
	if(x+picsize_w*step_x/gain < 0)
		return;
	if(y+picsize_h*step_y/gain < 0)
		return;

	show_x = x;
	show_y = y;
	
	for( i=0,py=0; i<picsize_h; py+=step_y,i=py/gain)
	{
		for( j=0,px=0; j<picsize_w; px+=step_x,j=px/gain)
		{
			if( p[j + i/8*picsize_w] & (0x01<<(i%8)) )
			{
				menu_write_point(target, show_x, show_y, 1);
			}
			else
			{
				if(GRAPHICSSHOWMANNER == GraphicsCover) //覆盖
					menu_write_point(target, show_x, show_y, 0);
			}
			show_x++;
		}
		show_x=x;
		show_y++;
	}
}




//========================== 线 立 方 体 ==============================

#define PI 3.1415926
#define SX 4
#define SY 6
#define DX PI / SX
#define DY PI * 2 / SY
#define X(a, b) (cx + v[a][b].x * r), (cy + v[a][b].y * r)

typedef struct { 
	double x, y;
} Vec;
static Vec v[SX + 1][SY + 1];

static void calc(double i, double j, double rot, Vec* v) {
    double x = sin(i) * cos(j), y = sin(i) * sin(j), z = cos(i),
        s = sin(rot), c = cos(rot), c1 = 1 - c, u = 1 / sqrt(3), u2 = u * u;
    v->x = x * (c + u2 * c1) + y * (u2 * c1 - u * s) + z * (u2 * c1 + u * s);
    v->y = x * (u2 * c1 + u * s) + y * (c + u2 * c1) + z * (u2 * c1 - u * s);
}


//线立方体 cx、cy：中心坐标  w、h宽高  rot：以0.1为单位递增即可
void Linecube(int cx, int cy, int w, int h, double rot)
{
	int  r = h * 0.375;
	for(int i = 0; i <= SX; ++i) for(int j = 0; j <= SY; ++j) 
			calc(i * DX, j * DY, rot, &v[i][j]);
			
	for(int i = 0; i < SX; ++i) for(int j = 0; j < SY; ++j) {
		DrawLine(X(i, j), X(i + 1, j));
		DrawLine(X(i, j), X(i, j + 1));
	}
}

//========================== 线 正 方 体 ==============================

// 线正方体 centerX、centerY 中心坐标 ， size：边长 ， rotx roty rotz：绕xyz轴的角度（弧度制）
void DrawCube(int centerX, int centerY, int size, float rotX, float rotY, float rotZ)
{
    // 计算立方体各个顶点的位置
    float halfSize = size / 2.0f;
    float vertices[8][3] = {
        {-halfSize, -halfSize, -halfSize},
        {halfSize, -halfSize, -halfSize},
        {halfSize, halfSize, -halfSize},
        {-halfSize, halfSize, -halfSize},
        {-halfSize, -halfSize, halfSize},
        {halfSize, -halfSize, halfSize},
        {halfSize, halfSize, halfSize},
        {-halfSize, halfSize, halfSize}
    };

    // 绕 X 轴旋转
    for (int i = 0; i < 8; i++) {
        float y = vertices[i][1];
        float z = vertices[i][2];
        vertices[i][1] = y * cos(rotX) + z * sin(rotX);
        vertices[i][2] = -y * sin(rotX) + z * cos(rotX);
    }

    // 绕 Y 轴旋转
    for (int i = 0; i < 8; i++) {
        float x = vertices[i][0];
        float z = vertices[i][2];
        vertices[i][0] = x * cos(rotY) + z * sin(rotY);
        vertices[i][2] = -x * sin(rotY) + z * cos(rotY);
    }

    // 绕 Z 轴旋转
    for (int i = 0; i < 8; i++) {
        float x = vertices[i][0];
        float y = vertices[i][1];
        vertices[i][0] = x * cos(rotZ) - y * sin(rotZ);
        vertices[i][1] = x * sin(rotZ) + y * cos(rotZ);
    }

    // 画立方体的边线
    DrawLine( centerX + vertices[0][0], centerY + vertices[0][1], centerX + vertices[1][0], centerY + vertices[1][1]);
    DrawLine(centerX + vertices[1][0], centerY + vertices[1][1], centerX + vertices[2][0], centerY + vertices[2][1]);
    DrawLine( centerX + vertices[2][0], centerY + vertices[2][1], centerX + vertices[3][0], centerY + vertices[3][1]);
    DrawLine(centerX + vertices[3][0], centerY + vertices[3][1], centerX + vertices[0][0], centerY + vertices[0][1]);
    DrawLine( centerX + vertices[4][0], centerY + vertices[4][1], centerX + vertices[5][0], centerY + vertices[5][1]);
    DrawLine( centerX + vertices[5][0], centerY + vertices[5][1], centerX + vertices[6][0], centerY + vertices[6][1]);
    DrawLine( centerX + vertices[6][0], centerY + vertices[6][1], centerX + vertices[7][0], centerY + vertices[7][1]);
		DrawLine(centerX + vertices[7][0], centerY + vertices[7][1], centerX + vertices[4][0], centerY + vertices[4][1]);
		DrawLine( centerX + vertices[0][0], centerY + vertices[0][1], centerX + vertices[4][0], centerY + vertices[4][1]);
		DrawLine(centerX + vertices[1][0], centerY + vertices[1][1], centerX + vertices[5][0], centerY + vertices[5][1]);
		DrawLine( centerX + vertices[2][0], centerY + vertices[2][1], centerX + vertices[6][0], centerY + vertices[6][1]);
		DrawLine(centerX + vertices[3][0], centerY + vertices[3][1], centerX + vertices[7][0], centerY + vertices[7][1]);
}

/*
	功能：进度条初始化
	barobj : 句柄
	weight : 宽度，正视屏幕
	high   : 高度，正视屏幕
	maxVal : 进度条满时最大值
	direction : 0-3， 显示方向，顺时针旋转
*/ 
ProgressBarTypedef * ProgressBarInit(ProgressBarTypedef *barobj, uint8_t direction, uint8_t width, uint8_t high, uint16_t maxVal)
{
	TanLevPIDInit(&barobj->pidhandle, 15, 0.5, 0.2, -0.2);
	
	barobj->BarVal = 0;
	barobj->maxVal = maxVal;
	barobj->width = width;
	barobj->high = high;
	barobj->R = (width<high?width:high)/2;
	barobj->animation = DISABLE;
	barobj->direction = direction%4;
	barobj->attribute = 0;
	return barobj;
}
/*
	功能：进度条显示
	barobj：句柄
	x：x坐标
	y：y坐标
	val：输入值，与maxVal比较计算填充比例

	ret: 指示器末端中心坐标

	注意：默认无动效，需自主使能 animation ，并定时调用
*/
int16_t * ProgressBar(ProgressBarTypedef *barobj, int16_t x, int16_t y, uint16_t Val)
{
	int16_t Barx,Bary;
	uint16_t Barwidth, Barhigh;

	int16_t dif;

	static int16_t xy[2];

	if(Val > barobj->maxVal) Val = barobj->maxVal; //限幅
	barobj->BarVal = Val;
	
	if(barobj->animation == DISABLE) //无过度动画
	{
		if(barobj->direction % 2 == 0)
			barobj->pixVal = (uint32_t)Val * (barobj->width - barobj->R*2)/ barobj->maxVal + barobj->R*2;
		else
			barobj->pixVal = (uint32_t)Val * (barobj->high - barobj->R*2)/ barobj->maxVal + barobj->R*2;
	}
	else    // 过度动画
	{
		//到达值
		if(barobj->direction % 2 == 0) //横向
			dif = (int32_t)Val * (barobj->width - barobj->R*2)/ barobj->maxVal + barobj->R*2;
		else
			dif = (int32_t)Val * (barobj->high - barobj->R*2)/ barobj->maxVal + barobj->R*2;

			barobj->pixVal = TandemLevel_PID(&barobj->pidhandle, dif, barobj->pixVal);

	}
	
	if(barobj->pixVal < barobj->R*2) barobj->pixVal = barobj->R*2; //最小指示
	
	switch(barobj->direction) //显示方向
	{
		case 0:
			Barx=x; Bary=y; Barwidth=barobj->pixVal; Barhigh=barobj->high;
			xy[0] = Barx + Barwidth;
			xy[1] = Bary + Barhigh/2;
		break;
		case 1:
			Barx=x; Bary=y; Barwidth=barobj->width; Barhigh=barobj->pixVal;
			xy[0] = Barx + Barwidth/2;
			xy[1] = Bary + Barhigh;
		break;
		case 2:
			Barx=x + barobj->width - barobj->pixVal; Bary=y; Barwidth=barobj->pixVal; Barhigh=barobj->high;
			xy[0] = Barx;
			xy[1] = Bary + Barhigh/2;
		break;
		case 3:
			Barx=x; Bary=y + barobj->high - barobj->pixVal; Barwidth=barobj->width; Barhigh=barobj->pixVal;
			xy[0] = Barx + Barwidth/2;
			xy[1] = Bary;
		break;
		default : break;
	}
	if( !(barobj->attribute & hideindicator ) )
		DrawfillRoundRect(Barx, Bary, Barwidth, Barhigh, barobj->R);
	if( !(barobj->attribute & hideframe ) )
		DrawRoundRect(x, y, barobj->width, barobj->high, barobj->R);

	return xy;
}
/*
	功能：获取bar的值
	barobj：句柄
*/
uint16_t getBarVal(ProgressBarTypedef *barobj)
{
	return barobj->BarVal;
}
//进度条属性设置
void SetProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute)
{
	barobj->attribute |= attibute;
}
//进度条属性复位
void ResProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute)
{
	barobj->attribute &= (~attibute);
}


/*
	功能：oled界面模糊
	n: 0-4  0不模糊 >=4类似清屏
*/

static const uint8_t BlurryData[]={0xff, 0x77, 0x33, 0x11, 0x00};

void InterfaceBlurry(uint8_t n)
{
	uint16_t i, j, k;

	if(n==0) return;
	
	n = n>4 ? 4 : n;

	for(i=0; i<ScreenPara.screenhigh; i++) //高度
	{
		for(j=0; j<ScreenPara.screenwidth; j+=4) //宽度
		{
			for(k=0; k<4 && (j+k)<SCREENWIDTH; k++)
			{
				if(k<4-n)
					DisplayBuff[i*SCREENWIDTH + j + k] &= BlurryData[n];
				else
					DisplayBuff[i*SCREENWIDTH + j + k] = 0x00;
			}
		}
	}
}

static const uint8_t quitMenuBlurryspeed[]  = {NULL, 1, 3, 4, 4}; // 1 -> *
static const uint8_t enterMenuBlurryspeed[] = {NULL, 1, 2, 3, 4}; // 1 <- *
/*
	功能：菜单进入、离开虚化

	state: 0 菜单截停  1 菜单虚化

	ret：1 开始虚化（处于当前菜单）  0 开始实化（处于切换后的菜单）

	注意：下函数内的 MaxCount 与上两数组关联，越大，切换的越慢、需要的时间越长。
				触发虚化时 输入响应 被禁止，内部主动改变指针时，请先判断是否能 输入响应，
				再改变菜单指针
				state=0 时，函数应放在   MenuAlwaysRun_PH 函数最开始处，
				state=1 时，函数尽量放在 MenuAlwaysRun_PL 函数结束处
*/
#include "menufontshow.h" //字符函数

uint8_t MenuDynamicBlurry(uint8_t state)
{
	const uint8_t MaxCount = 4;
	
	static menu_area *LastMenuTarget = NULL;
	static menu_area *NowMenuTarget = NULL;
	
	static uint8_t haveCush = 0; //是否有消息
	static uint8_t count=0;
	
	static uint32_t cushmes=0;
	
	switch(state)
	{
		case 0: //虚化
			
				if(haveCush==0 && cushIsNull() == false) //消息缓冲区不为空
				{
					cushmes = readCush();//获取缓冲区数据
					if(cushmes==Menu_Father || cushmes==Menu_Sub) //是否符合
					{
						ResponseEnable = DISABLE; //结束虚化前，禁止按键输入响应
						SetCushNull(true); //截停消息，清空标志
						count = 0;
						haveCush = 1; //开始虚化
					}
				}
				if(haveCush)
				{
					if(count == 0)
					{
						NowMenuTarget = TargetMenuPointrt.TargetMenuP;
						LastMenuTarget = TargetMenuPointrt.LastTargetMenuP;
						TargetMenuPointrt.TargetMenuP = LastMenuTarget;
					}
					if( (++count) > MaxCount)
					{
						cushMes(cushmes); //消息补发
						haveCush = 0;
						TargetMenuPointrt.TargetMenuP = NowMenuTarget;
						count--;
					}
				}
			break;
		
		default: // 实化
				if(haveCush)
				{
					InterfaceBlurry(quitMenuBlurryspeed[count]); // 1 -> *
				}
				else if(count)
				{
					InterfaceBlurry(enterMenuBlurryspeed[count--]); // * -> 1
					if(count==0)
					{
						ResponseEnable = ENABLE; // 重新使能 输入响应
					}
				}
			break;
	}
	return haveCush;
}





