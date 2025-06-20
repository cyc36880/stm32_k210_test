#include "Graphicalfunctions.h"
#include "menu_tool.h"

#include "math.h"

// ======================= ͼ �� �� �� �� ========================


static unsigned char ReadPoint(int16_t x, int16_t y)
{
	int px = x, py = (int)(y/8);
	unsigned char dat=0;
	
	if(x<0 || y<0) return 0;
	if(x>= SCREENWIDTH-1 || y>=SCREENHIGH) return 0; // x �� -1�����������Ҳ�һ�л��������������޸�
	
	dat = (DisplayBuff[px + py*SCREENWIDTH]>>(y%8)) & 0x01;
	
	return dat;
}

static void WritePoint(int16_t x, int16_t y, uint8_t w_d) 
{
	int px = x, py = (int)(y/8);
	
	if(x>=SCREENWIDTH-1|| y >= SCREENHIGH) return; // x �� -1�����������Ҳ�һ�л��������������޸�
	if(x<0 || y<0) return;
	
	if(w_d)
		DisplayBuff[px + py*SCREENWIDTH] |= (0x01 << (y % 8));
	else
		DisplayBuff[px + py*SCREENWIDTH] &= ~(0x01 << (y % 8));
}

// ����
unsigned char read_point(int16_t x, int16_t y)
{
	unsigned char point = 0;
	point = ReadPoint(x, y);
	return point;
}

// ����
void write_point(int16_t x, int16_t y, uint8_t w_d) 
{
	switch(GRAPHICSSHOWMANNER)
	{
		case GraphicsNormal:   
			WritePoint(x,  y,  w_d);
			break;
		case GraphicsRollColor: //��ת��ʾ
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

//������ٻ���
static void FastLineT(int16_t x, int16_t y, uint16_t w)
{
	while(w) {
		write_point(x+w-1, y, 1);
		w--;
	}
}
//������ٻ���
static void FastLineL(int16_t x, int16_t y, uint16_t h)
{
	while(h){
		write_point(x, y+h-1, 1);
		h--;
	}
}
//���ľ���
void DrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	if(h==0 || w==0) return;
	FastLineT(x, y, w);
	FastLineT(x, y+h-1, w);
	if(h<3) return;
	FastLineL(x, y+1, h-2);
	FastLineL(x+w-1, y+1, h-2);
}
//ʵ�ľ���
void DrawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	while(h) {
		FastLineT(x, y+h-1, w);
		h--;
	}
}

// ����
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{
		write_point(uRow,uCol, 1);//���� 
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
// ����Բ
void DrawCircle(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 
//ʵ��Բ
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		DrawLine(x0+a,y0-b,x0+a,y0+b);
		DrawLine(x0+b,y0-a,x0+b,y0+a);
		DrawLine(x0-a,y0-b,x0-a,y0+b);
		DrawLine(x0-b,y0-a,x0-b,y0+a); 	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
	}
}

// ----------------�� �� Բ �� �� ��------------
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
//ʵ��Բ�Ǿ���
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



//����α任
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
	centerX : ���ĵ�x����
	centerY �����ĵ�y����
	x       ��Ŀ���x����
	y       ��Ŀ���y����
	angle   ����ת�Ƕ�
	direct  ������ת 1˳ʱ�� 0��ʱ�� 
	����ֵ  ��unsigned int ָ�� 
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





//--------  �� �� ͼ ------------




//����ͼ�ڴ�����
void ClearnLineChartMapDat(TypLineChartMap *t)
{
	t->Startp = 0;
	t->Endp = 0;
	t->RxNum = 0;
	ClearnMemory(t->dat, sizeof(t->dat));
}


/* 
	���ܣ�������ͼ���������
	t������ͼָ��
	d��Ҫ��������

*/
void AddDatToLineChartMap(TypLineChartMap *t, uint16_t d)
{
	uint8_t width = t->width<128?t->width:128; //�������
	uint8_t high = t->high<128?t->high:128; //�߶�����
	
	if(++t->RxNum > 129) { //��������
		t->RxNum = 129;
	}
	if(d > t->max ) d = t->max; //�޷�
	d = (uint32_t) high * d / t->max ;
	
	t->dat[t->Endp] = d; //�������
	
	if(t->RxNum > width) { 
		if(++t->Startp >= 128) { // �ƶ���ʼλ��
			t->Startp = 0;
		}
	}
	if(++t->Endp >= 128) {
		t->Endp = 0;
	}
}

/*
	���ܣ�����ͼ����
	t������ͼָ��
	x��y����ʾ�����Ͻ�����
	@ret�����һ������������ͼ�е���ʾ����
*/
int16_t *LineChart(TypLineChartMap *t, int16_t x, int16_t y) 
{
	static int16_t xy[2] = {0, 0};
	int16_t posy = 0;   //��ʼλ�õ����y����
	int16_t posy_2 = 0; //����λ�õ����y����
	uint8_t Startp = 0; //������ʼ��ʾλ��
	uint8_t Startp_1 = 0;//���������ʾλ��
	
	uint8_t offset=0; //��x���ƫ��
	
	xy[0] = 0; //���������Ĭ��ֵ
	xy[1] = 0;
	
	if(t->RxNum == 0) { //�޽���
		
	}
	else if(t->RxNum == 1) { //������1������ʾ��
		
		posy = y + t->high - t->dat[0];
		write_point(x, posy, 1);
		
		xy[0] = x;
		xy[1] = posy;
	}
	else if(t->RxNum == 2) { //������2������ʾһ����
		posy = y + t->high - t->dat[0];   //y��������ת
		posy_2 = y + t->high - t->dat[1];
		DrawLine(x, posy, x+1, posy_2);
		
		xy[0] = x + 1;
		xy[1] = posy_2;
	}
	else { //���ն������ѭ����ʾ��������
		Startp = t->Startp;
		Startp_1 = Startp + 1 >= 128?0:Startp + 1;
		
		while(Startp_1 != t->Endp) {
			posy = y + t->high - t->dat[Startp]; //y��������ת
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



// ---------------- ͼ Ƭ -------------------

/*
	���ܣ�ͼƬ��ʾ

	size��ͼƬ�ߴ� max 255 X 255
	p��ͼƬָ��
	x��y������

	ע�⣺ȡģ��ʽ> ���� ����ʽ ����
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
	for(h=0; h<high; h++)//�ָ�
	{
		for(w=0; w<wight; w++) //�ֿ�
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
	ͼƬ��ʾ�����ٻ��ƣ�
	���޷��ڲ˵���������ʾ����������һ��
*/
static void FastDrawPic(const uint8_t *pic, int16_t x, int16_t y, uint16_t width, uint16_t high)
{	
	int16_t endx = x+width; //��ֹ��ʾ���½�����
	int16_t endy = y+high;
	int16_t beginx;
	uint8_t offset = y<0 ? (8+y%8)%8 : y%8; //ƫ����
	
	uint16_t i=0;
	
	if(endx<0 || endy<0) return; //������Ļ��ʾ��Χ������
	if(x>=SCREENWIDTH || y>=SCREENHIGH) return;
	
	if(endy > SCREENHIGH-1) endy=SCREENHIGH-1; //��ȥy�� ����δ��ʾ����
	if(y<=-8) { //��ȥy�� ����δ��ʾ����
		i+=width*( -(y/8) );
		y%=8;
	}
	
	for( ; y<endy; y+=8)
	{
		for(beginx = x; beginx<endx; beginx++, i++)
		{
			if(beginx<0) { //��ȥx�� ����δ��ʾ����
				i +=-beginx;
				beginx = 0;
			}
			if(beginx >= SCREENWIDTH-1) { //��ȥx�� ����δ��ʾ����
				i += endx - beginx;
				break;
			}
			
			switch(GRAPHICSSHOWMANNER)
			{
				case GraphicsRollColor: //��ɫ
					if(y>=0) 
					DisplayBuff[beginx+((y>>3) )*SCREENWIDTH] ^= pic[i]<<offset;
				
					if(y+8 <= SCREENHIGH-1 && offset) {
						if(y<0)
							DisplayBuff[beginx] ^= pic[i]>>(8-offset);
						else 
							DisplayBuff[beginx+((y>>3)+1 )*SCREENWIDTH] ^= pic[i]>>(8-offset);
					}
					break;
					
				case GraphicsCover: //����
					
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
	ͼƬ��ʾ�����ٻ��ƣ�
	��Ϊ����������ƥ�䣩���޷��ڲ˵���������ʾ����������һ��
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
#include "menufontshow.h" //�ַ�����
/*
	���ܣ�ͼƬ��ʾ
	ȡģ��ʽ������ ����ʽ ����

	ע�⣺��psizeΪNULL��ͼƬ��СӦ������p�У������С����psize���ã���ʵ��С���ú�����������ͼƬ��
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
	���ܣ�ͼ��������ʾ
	psize��ԭʼ�ߴ���Ϣ��ΪNULL��ʹ��p��ͷ��λ
	x��y���������Ļ������
	zf_x��zf_y:x��y��������ű�����100Ϊԭ�ߴ�
*/
void ImageScaling(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y, uint16_t zf_x, uint16_t zf_y)
{
	const uint16_t gain = 500; //���棨Խ�󣬱仯Խϸ�� ������100������Ҫ����65535��
	
	uint16_t i, j, px, py; //����ͼ��
	int16_t show_x, show_y;
	uint16_t picsize_w, picsize_h; //ͼ��Ŀ��
	uint16_t step_x, step_y; //���� 

	if(p == NULL) return;
	if(zf_x==0 || zf_y==0) return; //0���Ų���ʾ
	else if(zf_x==100 && zf_y==100) //ԭʼͼ����ʾ
	{
		DrawPicture(target, psize, p, x, y);
		return;
	}
	else
	{
		step_x = 100*gain/zf_x;//���� 
		step_y = 100*gain/zf_y; 
	}
	if(psize == NULL) //ȷ��ͼ��Ŀ��
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

	if(x>=SCREENWIDTH || y>=SCREENHIGH) //�ж��Ƿ�����Ļ����ʾ��Χ
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
				if(GRAPHICSSHOWMANNER == GraphicsCover) //����
					menu_write_point(target, show_x, show_y, 0);
			}
			show_x++;
		}
		show_x=x;
		show_y++;
	}
}




//========================== �� �� �� �� ==============================

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


//�������� cx��cy����������  w��h���  rot����0.1Ϊ��λ��������
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

//========================== �� �� �� �� ==============================

// �������� centerX��centerY �������� �� size���߳� �� rotx roty rotz����xyz��ĽǶȣ������ƣ�
void DrawCube(int centerX, int centerY, int size, float rotX, float rotY, float rotZ)
{
    // ������������������λ��
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

    // �� X ����ת
    for (int i = 0; i < 8; i++) {
        float y = vertices[i][1];
        float z = vertices[i][2];
        vertices[i][1] = y * cos(rotX) + z * sin(rotX);
        vertices[i][2] = -y * sin(rotX) + z * cos(rotX);
    }

    // �� Y ����ת
    for (int i = 0; i < 8; i++) {
        float x = vertices[i][0];
        float z = vertices[i][2];
        vertices[i][0] = x * cos(rotY) + z * sin(rotY);
        vertices[i][2] = -x * sin(rotY) + z * cos(rotY);
    }

    // �� Z ����ת
    for (int i = 0; i < 8; i++) {
        float x = vertices[i][0];
        float y = vertices[i][1];
        vertices[i][0] = x * cos(rotZ) - y * sin(rotZ);
        vertices[i][1] = x * sin(rotZ) + y * cos(rotZ);
    }

    // ��������ı���
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
	���ܣ���������ʼ��
	barobj : ���
	weight : ��ȣ�������Ļ
	high   : �߶ȣ�������Ļ
	maxVal : ��������ʱ���ֵ
	direction : 0-3�� ��ʾ����˳ʱ����ת
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
	���ܣ���������ʾ
	barobj�����
	x��x����
	y��y����
	val������ֵ����maxVal�Ƚϼ���������

	ret: ָʾ��ĩ����������

	ע�⣺Ĭ���޶�Ч��������ʹ�� animation ������ʱ����
*/
int16_t * ProgressBar(ProgressBarTypedef *barobj, int16_t x, int16_t y, uint16_t Val)
{
	int16_t Barx,Bary;
	uint16_t Barwidth, Barhigh;

	int16_t dif;

	static int16_t xy[2];

	if(Val > barobj->maxVal) Val = barobj->maxVal; //�޷�
	barobj->BarVal = Val;
	
	if(barobj->animation == DISABLE) //�޹��ȶ���
	{
		if(barobj->direction % 2 == 0)
			barobj->pixVal = (uint32_t)Val * (barobj->width - barobj->R*2)/ barobj->maxVal + barobj->R*2;
		else
			barobj->pixVal = (uint32_t)Val * (barobj->high - barobj->R*2)/ barobj->maxVal + barobj->R*2;
	}
	else    // ���ȶ���
	{
		//����ֵ
		if(barobj->direction % 2 == 0) //����
			dif = (int32_t)Val * (barobj->width - barobj->R*2)/ barobj->maxVal + barobj->R*2;
		else
			dif = (int32_t)Val * (barobj->high - barobj->R*2)/ barobj->maxVal + barobj->R*2;

			barobj->pixVal = TandemLevel_PID(&barobj->pidhandle, dif, barobj->pixVal);

	}
	
	if(barobj->pixVal < barobj->R*2) barobj->pixVal = barobj->R*2; //��Сָʾ
	
	switch(barobj->direction) //��ʾ����
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
	���ܣ���ȡbar��ֵ
	barobj�����
*/
uint16_t getBarVal(ProgressBarTypedef *barobj)
{
	return barobj->BarVal;
}
//��������������
void SetProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute)
{
	barobj->attribute |= attibute;
}
//���������Ը�λ
void ResProBarAttibute(ProgressBarTypedef *barobj, uint8_t attibute)
{
	barobj->attribute &= (~attibute);
}


/*
	���ܣ�oled����ģ��
	n: 0-4  0��ģ�� >=4��������
*/

static const uint8_t BlurryData[]={0xff, 0x77, 0x33, 0x11, 0x00};

void InterfaceBlurry(uint8_t n)
{
	uint16_t i, j, k;

	if(n==0) return;
	
	n = n>4 ? 4 : n;

	for(i=0; i<ScreenPara.screenhigh; i++) //�߶�
	{
		for(j=0; j<ScreenPara.screenwidth; j+=4) //���
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
	���ܣ��˵����롢�뿪�黯

	state: 0 �˵���ͣ  1 �˵��黯

	ret��1 ��ʼ�黯�����ڵ�ǰ�˵���  0 ��ʼʵ���������л���Ĳ˵���

	ע�⣺�º����ڵ� MaxCount ���������������Խ���л���Խ������Ҫ��ʱ��Խ����
				�����黯ʱ ������Ӧ ����ֹ���ڲ������ı�ָ��ʱ�������ж��Ƿ��� ������Ӧ��
				�ٸı�˵�ָ��
				state=0 ʱ������Ӧ����   MenuAlwaysRun_PH �����ʼ����
				state=1 ʱ�������������� MenuAlwaysRun_PL ����������
*/
#include "menufontshow.h" //�ַ�����

uint8_t MenuDynamicBlurry(uint8_t state)
{
	const uint8_t MaxCount = 4;
	
	static menu_area *LastMenuTarget = NULL;
	static menu_area *NowMenuTarget = NULL;
	
	static uint8_t haveCush = 0; //�Ƿ�����Ϣ
	static uint8_t count=0;
	
	static uint32_t cushmes=0;
	
	switch(state)
	{
		case 0: //�黯
			
				if(haveCush==0 && cushIsNull() == false) //��Ϣ��������Ϊ��
				{
					cushmes = readCush();//��ȡ����������
					if(cushmes==Menu_Father || cushmes==Menu_Sub) //�Ƿ����
					{
						ResponseEnable = DISABLE; //�����黯ǰ����ֹ����������Ӧ
						SetCushNull(true); //��ͣ��Ϣ����ձ�־
						count = 0;
						haveCush = 1; //��ʼ�黯
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
						cushMes(cushmes); //��Ϣ����
						haveCush = 0;
						TargetMenuPointrt.TargetMenuP = NowMenuTarget;
						count--;
					}
				}
			break;
		
		default: // ʵ��
				if(haveCush)
				{
					InterfaceBlurry(quitMenuBlurryspeed[count]); // 1 -> *
				}
				else if(count)
				{
					InterfaceBlurry(enterMenuBlurryspeed[count--]); // * -> 1
					if(count==0)
					{
						ResponseEnable = ENABLE; // ����ʹ�� ������Ӧ
					}
				}
			break;
	}
	return haveCush;
}





