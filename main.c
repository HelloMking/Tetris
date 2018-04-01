////////////////////////////////////////////头文件包含

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "gotoxy.h"

#include "bioskey.h"
#include <string.h>

/////////////////////////////////////////宏定义
//键盘按键返回值定义
#define LEFT 7500
#define RIGHT 7700
#define UP 7200
#define DOWN 8000
#define ENTER 13
//全局相对坐标远点
#define O1 20
#define O2 1
//二维数组大小
#define X 20
#define Y 22
//线程API
DWORD WINAPI ThreadProc(
  LPVOID lpParameter   
);
////////////////////////////////////////////全局变量定义

//方块图形缓存
struct BLOCK
{
	int x[4];
	int y[4];
}block[19];

//排行榜 结构体
struct data
{
	int no;
	char name[15];
	long int score;
	struct data *next;
}rec[11],*head=NULL;

	int a[X][Y]={0},b[X][Y]={0},c[4][2]={0},next[8][4], /*h[X/2]={0},*/now[8][4],Now=-1,net=-1;
	//  已下落方块  正下落方块  方块缓存	即将下落方块 高度临时记录 当前方块 当前/下一个方块编号

	int hard=2;								//游戏难度
	long int randseed;						//随机数种子
	HANDLE hand;							//线程变量
	long int final_score;					//最终得分
	long int TIME;							//时间变量

/////////////////////////////函数声明
	int bioskey(int);						//按键记录函数，包含于bioskey.h
	void Iblock(void);						//初始化方块坐标
	void gamestart(int);					//游戏开始模块
	int change (int key);					//方块变化
	void clean (int *,int ,int);			//清除屏幕图形
	void draw(int *,int,int,int,int,int);	//绘制屏幕图形
	int radblock(int);						//产生随机方块编号
	void copyb(void);						//将方块缓存复制到b数组
	int remove(void);						//方块消除
	int downto(void);						//方块下落
	int win(void);							//胜负判断
	void nexttonow(void);					//把下一个方块复制到当前
	void drawnext(void);					//绘制下一个方块
	void menum(void);						//绘制游戏主界面
	void cleanall(void);					//清除屏幕内容
	void record (void);						//排行记录
	int record_read(void);					//读取记录
	void record_show(void);					//显示排行
	int record_edit(void);					//新纪录排序
	int record_save(void);					//保存排行
	int menu(void);							//主菜单
	int menu_select(void);					//主菜单选择
	void menu_clean(int);					//清除屏幕菜单
	void menu_hard(void);					//难度菜单
	int menu_hard_select(int);				//难度菜单选择
	int keypress(void);						//按键监视函数
	void encrypt(char *);

/////////////////////////////////////////////////////主函数开始
void main()
{



///////////////变量声明
	int i;									//循环变量
	long int score;							//分数
	long int _t,old_t;						//当前/上一次 时间
	char title[25]={73,18,61,33,52,70,72,66,64,22,-33,-67,-90,92,69,69,81,73,83,72,54,46,86,-1};

///////////////程序开始
	encrypt(title);

	SetConsoleTitle(title);
	setcolor(10,0);
	clrscr();
	randseed=(unsigned)time( NULL );					//初始化随机数种子
	hand=CreateThread(NULL,0,ThreadProc,NULL,0,NULL);	//创建线程
	ZeroMemory(rec,sizeof(rec));						//变量清零
	
	Iblock();						//初始化方块存储变量
	record_read();
	while(1)
	{		
	menum();						//主游戏界面
	menu();
	gamestart(hard);
	cleanall();
	}

	CloseHandle(hand);				//关闭线程
}

///////////////////////擦除方块
void clean(int *m ,int x,int y)
{
	int i,k;
	
		for (i=0;i<x;i=i+2)
		
		{
			for (k=0;k<y;k++)
			{
				if (m[i*y+k]==1)
				{
					if(y>=3)
					{
						gotoxy((i+O1),(k+O2));
						puts("  ");
					}
					
				}
			}
		}
}

////////////////////////重绘图形
void draw(int *p,int n,int x,int y,int x1,int y1)
{
	int i,k;
	
		for (i=0;i<x;i=i+2)
		
		{
			for (k=0;k<y;k++)
			{
				if (p[i*y+k]==1)
				{
					if (k>=4)
					{
						gotoxy((i+x1),(k+y1));
						if(n==0)
						{
							puts("□");
						}
						else
							puts("■");
					}
					
				}
			}
		}
}




//////////////////////////////////图形变换
int change (int key)
{
	int i,k,t,flg=0;						//临时/循环/状态变量
	int score;								//分数
	int bottom=-1,top=-1,left=-1,right=-1;	//边界记录
	int x[4]={0},y[4]={0},tmp=0,min;		//方块坐标
	int px=0,py=0;							//旋转中心坐标
	
	tmp=0;
	for(i=0;i<X;i++)
		for(k=0;k<Y;k++)
		{
			if(b[i][k]==1)
			{
				x[tmp]=i;
				y[tmp]=k;
				tmp++;
			}
			
		}
	for(i=0;i<X;i++)				//循环图形左边界
		for(k=0;k<Y;k++)
		{
			if(b[i][k]==1)
			{
				left=i;
				break;
			}
			if(left!=-1) break;
		}

	for(i=X-2;i>=0;i--)				//循环图形右边界
		for(k=0;k<Y;k++)
		{
			if(b[i][k]==1)
			{
				right=i;
				break;
			}
			if(right!=-1) break;
		}

	for(k=0;k<Y;k++)				//循环图形上边界
		for(i=0;i<X;i=i+2)
		{
			if(b[i][k]==1)
			{
				top=k;
				break;
			}
			if(top!=-1) break;
		}
	for(k=Y-1;k>=0;k--)				//循环图形下边界
		for(i=0;i<X;i=i+2)
		{
			if(b[i][k]==1)
			{
				bottom=k;
				break;
			}
			if(bottom!=-1) break;
		}


	switch (key)					//判断按键
	{
		case LEFT:					//按下方向 左 键
			{
				clean(&b[0][0],X,Y);

					if (left==0)	//判断是否处于左边界
					{
						flg=1;
					}
				

				for(i=0;i<4;i++)	//判断左方是否有图形
				{
					if(x[i]>0 && a[x[i]-2][y[i]]==1)
					{
						flg=1;
						break;
					}
				}
						




				if (flg==0)			//左移图形
				{
					for(i=0;i<X;i++)
						for(k=0;k<Y;k++)
						{
							if(b[i][k]==1)
							{
								b[i-2][k]=1;
								b[i][k]=0;
							}
						}
					
				}
				else flg=0;
				draw(&b[0][0],0,X,Y,O1,O2);	//图形重绘
				return 0;
				break;
			}
		case RIGHT:
			{
				clean(&b[0][0],X,Y);
				if (right>=X-2)			//判断图形处于右边界
					{
						flg=1;
					}
				
				for(i=0;i<4;i++			)//判断右边是否有图形
				{
					if(x[i]<X-1 && a[x[i]+2][y[i]]==1)
					{
						flg=1;
						break;
					}
				}
				

			if (flg==0)
			{
				for(i=Y-1;i>=0;i--)
					for(k=X-1;k>=0;k--)
					{
						if (b[k][i]==1)
						{
							b[k+2][i]=1;
							b[k][i]=0;
						}
					}
			}
			else flg=0;
			draw(&b[0][0],0,X,Y,O1,O2);
			return 0;
			break;
			
			}

		case UP:
			{
				switch (Now)				//判断图形
				{
				case 0:
					{
						if (right==X-2)		//定位旋转点，下同
						{
							px=right-2;
							py=bottom-1;
						}
						else if(left==0)
						{
							px=right;
							py=bottom;
						}
						else
						{
							px=right;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )//判断旋转空间
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(1);
							Now=1;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];

						}
						break;
					}



					
				case 1:
					{		
						if(left==0)
						{
							px=right-2;
							py=bottom;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<=Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);

							ZeroMemory(b,sizeof(b));
							radblock(2);
							Now=2;

							b[px-2][py-1]=0;
							b[px][py-1]=now[0][1];
							b[px+2][py-1]=now[2][1];

							b[px-2][py]=0;
							b[px][py]=now[0][2];
							b[px+2][py]=now[2][2];

							b[px-2][py+1]=0;
							b[px][py+1]=now[0][3];
							b[px+2][py+1]=now[2][3];

						}
						break;
					}
				case 2:
					{
						if (left==0)
						{
							px=left+2;
							py=bottom+1;
						}
						else if(right==X-2)
						{
							px=left;
							py=bottom-1;
						}
						else
						{
							px=left;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(3);
							Now=3;

							b[px-2][py-1]=now[0][2];
							b[px][py-1]=now[2][2];
							b[px+2][py-1]=now[4][2];

							b[px-2][py]=now[0][3];
							b[px][py]=now[2][3];
							b[px+2][py]=now[4][3];

							b[px-2][py+1]=0;
							b[px][py+1]=0;
							b[px+2][py+1]=0;

						}
						break;
					}
				case 3:
					{
							px=right-2;
							py=bottom;
						

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(0);
							Now=0;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];

						}
						break;
					}
/////////////////////////////图形1结束//////////////////////////
				case 4:
					{
						if (right==X-2)
						{
							px=right-2;
							py=bottom-1;
						}
						else if (left==0)
						{
							px=right;
							py=bottom;
						}
						else
						{
							px=right;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(5);
							Now=5;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;


					}
				case 5:
					{
						if(left==0)
						{
							px=right-2;
							py=bottom;
						}else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(4);
							Now=4;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}

						break;
					}
////////////////////////////////图形2结束////////////////////////////
				case 6:
					{
						if (right==X-2)
						{
							px=right-2;
							py=bottom-1;
						}
						else if (left==0)
						{
							px=right;
							py=bottom;
						}
						else
						{
							px=right;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(7);
							Now=7;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;
					}
				case 7:
					{
						if(left==0)
						{
							px=right-2;
							py=bottom;
						}else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(6);
							Now=6;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}

						break;
					}
/////////////////////////////图形3结束//////////////////////////////////
				case 8:
					{
						if (right==X-2)
						{
							px=right-2;
							py=bottom-1;
						}
						else if (left==0)
						{
							px=right;
							py=bottom;
						}
						else
						{
							px=right;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(9);
							Now=9;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;
					}
				case 9:
					{
						if (left==0)
						{
							px=right-2;
							py=bottom;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(10);
							Now=10;

							b[px-2][py-1]=0;
							b[px][py-1]=now[0][1];
							b[px+2][py-1]=now[2][1];

							b[px-2][py]=0;
							b[px][py]=now[0][2];
							b[px+2][py]=now[2][2];

							b[px-2][py+1]=0;
							b[px][py+1]=now[0][3];
							b[px+2][py+1]=now[2][3];
						}
						break;
					}
				case 10:
					{
						if (left==0)
						{
							px=right;
							py=bottom-1;
						}
						else if (right==X-2)
						{
							px=right-2;
							py=bottom-2;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(11);
							Now=11;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;
					}
				case 11:
					{
						if (right==X-2)
						{
							px=right-2;
							py=bottom-1;
						}
						else if (left==0)
						{
							px=right-2;
							py=bottom;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(8);
							Now=8;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;
					}
////////////////////////////////图形4结束////////////////////////////////
				case 12:
					{
						if (right==X-2)
						{
							px=right-2;
							py=bottom;
						}
						else if (left==0)
						{
							px=right;
							py=bottom;
						}
						else
						{
							px=right;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(13);
							Now=13;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;
					}
				case 13:
					{
						if (left==0)
						{
							px=right-2;
							py=bottom;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(14);
							Now=14;

							b[px-2][py-1]=0;
							b[px][py-1]=now[0][1];
							b[px+2][py-1]=now[2][1];

							b[px-2][py]=0;
							b[px][py]=now[0][2];
							b[px+2][py]=now[2][2];

							b[px-2][py+1]=0;
							b[px][py+1]=now[0][3];
							b[px+2][py+1]=now[2][3];
						}
						break;
					}
				case 14:
					{
						if (left==0)
						{
							px=right;
							py=bottom-1;
						}
						else if (right==X-2)
						{
							px=right-2;
							py=bottom-1;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(15);
							Now=15;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;
					}
				case 15:
					{
						if (right==X-2)
						{
							px=right-2;
							py=bottom-1;
						}
						else if (left==0)
						{
							px=right-2;
							py=bottom;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && a[px][py]==0 && a[px-2][py]==0 && a[px+2][py]==0 && a[px][py-1]==0 && a[px-2][py-1]==0 && a[px+2][py-1]==0 && a[px][py+1]==0 && a[px-2][py+1]==0 && a[px+2][py+1]==0 )
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(12);
							Now=12;

							b[px-2][py-1]=now[0][1];
							b[px][py-1]=now[2][1];
							b[px+2][py-1]=now[4][1];

							b[px-2][py]=now[0][2];
							b[px][py]=now[2][2];
							b[px+2][py]=now[4][2];

							b[px-2][py+1]=now[0][3];
							b[px][py+1]=now[2][3];
							b[px+2][py+1]=now[4][3];
						}
						break;
					}
/////////////////////////////////////图形5结束////////////////////////
				case 17:
					{
					//	if (hard==3) break;		//最高难度

						if (right==X-2)
						{
							px=right-2;
							py=bottom;
						}
						else if (left==0)
						{
							px=right;
							py=bottom;
						}
						else
						{
							px=right;
							py=bottom;
						}

						if (py+1<Y && !a[px+2][py] && !a[px][py] && !a[px-2][py] && !a[px-4][py] && !a[px+2][py-1] && !a[px][py-1] && !a[px-2][py-1] && !a[px-4][py-1]  && !a[px+2][py+1] && !a[px][py+1] && !a[px-2][py+1] && !a[px-4][py+1] && !a[px+2][py-2] && !a[px][py-2] && !a[px-2][py-2] && !a[px-4][py-2])
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(18);
							Now=18;

							b[px-4][py-2]=0;
							b[px-2][py-2]=0;
							b[px][py-2]=now[0][0];
							b[px+2][py-2]=0;

							b[px-4][py-1]=0;
							b[px-2][py-1]=0;
							b[px][py-1]=now[0][1];
							b[px+2][py-1]=0;

							b[px-4][py]=0;
							b[px-2][py]=0;
							b[px][py]=now[0][2];
							b[px+2][py]=0;

							b[px-4][py+1]=0;
							b[px-2][py+1]=0;
							b[px][py+1]=now[0][3];
							b[px+2][py+1]=0;


						}
						break;
					}
				case 18:
					{
					//	if (hard==3) break;

						if (right==X-2)
						{
							px=right-2;
							py=bottom-1;
						}
						else if (left<=4)
						{
							px=4;
							py=bottom+2;
						}
						else
						{
							px=right-2;
							py=bottom-1;
						}

						if (py+1<Y && !a[px+2][py] && !a[px][py] && !a[px-2][py] && !a[px-4][py] && !a[px+2][py-1] && !a[px][py-1] && !a[px-2][py-1] && !a[px-4][py-1]  && !a[px+2][py+1] && !a[px][py+1] && !a[px-2][py+1] && !a[px-4][py+1] && !a[px+2][py-2] && !a[px][py-2] && !a[px-2][py-2] && !a[px-4][py-2])
						{
							clean(&b[0][0],X,Y);
							ZeroMemory(b,sizeof(b));
							radblock(17);
							Now=17;

							b[px-4][py-2]=0;
							b[px-2][py-2]=0;
							b[px][py-2]=0;
							b[px+2][py-2]=0;

							b[px-4][py-1]=0;
							b[px-2][py-1]=0;
							b[px][py-1]=0;
							b[px+2][py-1]=0;

							b[px-4][py]=now[0][3];
							b[px-2][py]=now[2][3];
							b[px][py]=now[4][3];
							b[px+2][py]=now[6][3];

							b[px-4][py+1]=0;
							b[px-2][py+1]=0;
							b[px][py+1]=0;
							b[px+2][py+1]=0;


						}
						break;
					}
					
				}

				draw(&b[0][0],0,X,Y,O1,O2);
				break;
				



			}

		case DOWN:
			{
				/*min=h[x[0]/2]-y[0];//直接到底的处理方式，不完善，存在bug。

				clean(&b[0][0],X,Y);
				for(i=0;i<4;i++)
				{
					if ((h[x[i]/2]-y[i])<min)
						min=h[x[i]/2]-y[i];
				}

				clean(&a[0][0],X,Y);

				for(i=0;i<4;i++)
				{
					a[(x[i])][(y[i]+min-1)]=b[x[i]][y[i]];
					
				}
				socer=remove();
				draw(&a[0][0],1,X,Y,O1,O2);
				

				for(i=0;i<X;i++)  //b数组清零
					for(k=0;k<Y;k++)
					{
						b[i][k]=0;
					}

				
				nexttonow();
				copyb();
				net=radblock(-1);
				drawnext();
				
				draw(&b[0][0],0,X,Y,O1,O2);
				return socer;
				break;*/
				score=downto();
				return score;
					

					break;

					
				
				
					
				




			}


					
		default: 
			break;
	}
	return 0;





}

/////////////////////////////////结束//////////////////////////////

int win()
{
	int i;
	for(i=0;i<X;i++)
	{
		if(a[i][4]==1) return 1;
		
	}
	return 0;

}

int downto()
{
	int i,k,tmp=0,x[4]={0},y[4]={0},flg=1,socer;
	clean(&b[0][0],X,Y);
	tmp=0;
	socer=0;

	for(i=0;i<X;i++)
		for(k=0;k<Y;k++)
		{
			if(b[i][k]==1)
			{
				x[tmp]=i;
				y[tmp]=k;
				tmp++;
			}
			
		}

	for(i=0;i<4;i++)
	{
		if(a[x[i]][y[i]+1]==1 || y[i]>=Y-2)
		{
			flg=0;
			break;
		}
	}

	if(flg==1)
	{
		for(i=Y-1;i>=0;i--)
		{
			for(k=0;k<X;k++)
			{
				if(b[k][i]==1)
				{
					b[k][i]=0;
					b[k][i+1]=1;
				}
			}
			
		}
	}
	else
	{
		clean(&a[0][0],X,Y);

		for(i=0;i<4;i++)
		{
			a[(x[i])][y[i]]=b[x[i]][y[i]];					
		}
		socer=remove();
		draw(&a[0][0],1,X,Y,O1,O2);
		

		for(i=0;i<X;i++)  //b数组清零
			for(k=0;k<Y;k++)
			{
				b[i][k]=0;
			}
		
		nexttonow();
		
		net=radblock(-1);
		copyb();
		drawnext();
	
		flg=1;

	}
	draw(&b[0][0],0,X,Y,O1,O2);
	return socer;




}

int remove()
{
	int i=0,k=0,m,n,sum=0,flg=1;

	i=0;
	k=0;		//循环变量
	sum=0;		//消去的行数
	flg=1;		//是否可消除

	for(i=Y-1;i>=0;i--)
	{
		for(k=0;k<X;k+=2)
		{
			if(a[k][i]==0)
			{
				flg=0;
				break;
			}
		}
		if(flg==1)
		{
			for(m=0;m<X;m++)
				a[m][i]=0;
			sum++;
			for(m=0;m<X;m+=2)
			{
				for(n=i;n>=0;n--)
				{
					if(a[m][n]==1)
					{
						a[m][n]=0;
						a[m][n+1]=1;
					}
				}
			}
			i++;
		}
		else
		{
			flg=1;
		}
	}

	switch (sum)
	{
	case 0:return 0;
	case 1:return 1*hard;
	case 2:return 3*hard;
	case 3:return 5*hard;
	case 4:return 10*hard;
	}
}


/////////////////界面初始化

void menum(void)
{
	int i,k;

	gotoxy(O1-2,O2+4);
	printf("┏");

	for(i=1;i<Y-5;i++)
	{
		gotoxy(O1-2,O2+i+4);
		printf("┃");
	}

	gotoxy(O1-2,O2+i+4);
	printf("┗━━━━━━━━━━");

	gotoxy(O1+X,O2+4);
	printf("┳");

	for(i=1;i<Y-5;i++)
	{
		gotoxy(O1+X,O2+i+4);
		printf("┃");
	}

	gotoxy(O1+X,O2+i+4);
	printf("┻━━━━━");

	gotoxy(O1+X+2,O2+4);
	printf("━━━━━┓");

	gotoxy(O1+X+12,O2+5);
	printf("┃");

	gotoxy(O1+X+12,O2+6);
	printf("┃");

	gotoxy(O1+X+12,O2+7);
	printf("┃");

	gotoxy(O1+X+12,O2+8);
	printf("┃");

	gotoxy(O1+X+12,O2+9);
	printf("┫");

	gotoxy(O1+X,O2+9);
	printf("┣━━━━━");

	gotoxy(O1+X+12,O2+10);
	printf("┃");

	for(k=6;k<Y-5;k++)
	{
		gotoxy(O1+X+12,O2+k+4);
		printf("┃");
	}

	gotoxy(O1+X+12,O2+k+4);
	printf("┛");

	gotoxy(O1+X+2,O2+10);
	printf("分数：");
	gotoxy(O1+X+4,O2+11);
	printf("    ");
	gotoxy(O1+X+2,O2+15);
	printf("Q:结束游戏");
	gotoxy(O1+X+2,O2+17);
	printf("P:暂停游戏");

	



}


		







int radblock(int m)
{
	int i,k;
	
	if(m==-1)
	{
		ZeroMemory(next,sizeof(next));
		srand( randseed );
		m=rand()%19;
		
		for (i=0;i<4;i++)
		{
			c[i][0]=block[m].x[i];
			c[i][1]=block[m].y[i];
		
			//now[c[i][0]][c[i][1]]=1;
		
			next[c[i][0]][c[i][1]]=1;
		}
	}
	else
	{
		ZeroMemory(now,sizeof(now));
				for (i=0;i<4;i++)
		{
			c[i][0]=block[m].x[i];
			c[i][1]=block[m].y[i];
			now[c[i][0]][c[i][1]]=1;
		}
	}

	
	return (m);
	
}

void drawnext()
{
	int i,k;
	for (i=0;i<8;i++)
		for(k=0;k<4;k++)
		{
			gotoxy((O1+X+2+i),(O2+k+5));
			printf(" ");
		}


	for (i=0;i<8;i++)
		for(k=0;k<4;k++)
		{
			if(next[i][k]==1)
			{
				gotoxy((O1+X+2+i),(O2+k+5));
				printf("□");
			}
		}
}

void nexttonow()
{
	int i,k;

	ZeroMemory(now,sizeof(now));
	for (i=0;i<8;i++)
		for(k=0;k<4;k++)
		{
			now[i][k]=next[i][k];
		}
	Now=net;
}

void copyb()
{
	int i,k;
	for (i=0;i<8;i++)
		for(k=0;k<4;k++)
		{
			b[i+8][k]=now[i][k];
		}
}

void gamestart(int hard)
{
	int keypress(void);
	long int score;
	long int _t,old_t;
	int time_hard;
	int key;
	

	net=radblock(-1);
	nexttonow();
	Sleep(10);
	net=radblock(-1);
	copyb();
	drawnext();
	clean(&b[0][0],X,Y);
	draw(&b[0][0],0,X,Y,O1,O2);
	final_score=score=0;				//分数初始化

	gotoxy(O1+X+2,O2+10);				//输出初始分数
	printf("分数：");
	gotoxy(O1+X+4,O2+11);
	printf("%4ld",score);

	switch(hard)						//计算游戏难度
	{
	case 1:time_hard=100;break;
	case 2:time_hard=50;break;
	case 3:time_hard=20;break;
	}


	while(1)
	{
		if((key=keypress())==1) return;
		score+=change (key);
		_t=TIME;
//////////下落计时/////////////		
		if (_t%time_hard==0 && old_t!=_t)
		{
			score+=downto();
			old_t=_t;
		}
////////////////////得分判断//////////
		if(final_score!=score)
		{
			gotoxy(O1+X+2,O2+10);
			printf("分数：");
			gotoxy(O1+X+4,O2+11);
			printf("%4ld",score);
			final_score=score;
			if(hard==1 && final_score>=500)
			{
				gotoxy(O1+X/2-7,O2+Y/2-1);
				printf("----------------");
				gotoxy(O1+X/2-3,O2+Y/2);
				printf("游戏结束！");
				gotoxy(O1+X/2-7,O2+Y/2+1);
				printf("称号：方块新秀");
				gotoxy(O1+X/2-7,O2+Y/2+2);
				printf("----------------");
				getch();

				return;
			}
			else if(hard==2 && final_score>=3000)
			{
				gotoxy(O1+X/2-7,O2+Y/2-1);
				printf("----------------");
				gotoxy(O1+X/2-3,O2+Y/2);
				printf("游戏结束！");
				gotoxy(O1+X/2-7,O2+Y/2+1);
				printf("称号：方块高玩");
				gotoxy(O1+X/2-7,O2+Y/2+2);
				printf("----------------");
				getch();

				return;
			}
			else if(hard==3 && final_score>=99990)
			{
				gotoxy(O1+X/2-7,O2+Y/2-1);
				printf("----------------");
				gotoxy(O1+X/2-3,O2+Y/2);
				printf("游戏结束！");
				gotoxy(O1+X/2-7,O2+Y/2+1);
				printf("称号：传说中的神");
				gotoxy(O1+X/2-7,O2+Y/2+2);
				printf("----------------");
				getch();

				return;
			}
		}
		if(win()) break;
		Sleep(1);
		
		
	}


	gotoxy(O1+X+2,O2+13);
	printf("%s","游戏结束 !");
	gotoxy(0,0);
	getch();
	record();
	gotoxy(O1+X+2,O2+13);
	printf("%s","          ");

}

void menu_clean(int all)//清除屏幕菜单
{
	if(all)
	{
		gotoxy(O1+X/2-6,O2+Y/2);
			printf("          ");
		gotoxy(O1+X/2-6,O2+Y/2+1);
			printf("          ");
		gotoxy(O1+X/2-6,O2+Y/2+2);
			printf("          ");
		gotoxy(O1+X/2-6,O2+Y/2+3);
			printf("          ");	
	}
	else
	{
		gotoxy(O1+X/2-6,O2+Y/2);
			printf("    ");
		gotoxy(O1+X/2-6,O2+Y/2+1);
			printf("    ");
		gotoxy(O1+X/2-6,O2+Y/2+2);
			printf("    ");
		gotoxy(O1+X/2-6,O2+Y/2+3);
			printf("    ");
	}
}


int menu ()
{
	
	int menu_select(void);
	void menu_hard(void);
	void record_show(void);

	while(1)
	{
		gotoxy(O1+X/2-2,O2+Y/2);
			printf("1.开始");
		gotoxy(O1+X/2-2,O2+Y/2+1);
			printf("2.难度");
		gotoxy(O1+X/2-2,O2+Y/2+2);
			printf("3.排行");
		gotoxy(O1+X/2-2,O2+Y/2+3);
			printf("4.退出");
		switch(menu_select())
		{
		case 1:
			menu_clean(1);
			return;
			break;
		case 2:menu_hard();break;
		case 3:record_show();break;
		case 4:CloseHandle(hand);exit(1);break;
		default:{break;}
		}
	}
}

int menu_select()
{
	int selected=1;
	int keypressed;

	gotoxy(O1+X/2-6,O2+Y/2);
	printf("->");
	while(1)
	{
		gotoxy(0,0);
		keypressed=bioskey(1);

		switch(keypressed)
		{
		case UP:
			{
				if(selected==1)
				{
					selected=4;
					break;
				}
				else
					selected--;
				break;
			}
		case DOWN:
			{
				if(selected==4)
				{
					selected=1;
					break;
				}
				else
					selected++;
				break;
			}
		case ENTER:
			{
				return selected;
				break;
			}
		default:
			
				break;
			
		}
		
		menu_clean(0);

		switch(selected)
		{
		case 1:
			{

				gotoxy(O1+X/2-6,O2+Y/2);
				printf("->");
				break;
			}

		case 2:
			{

				gotoxy(O1+X/2-6,O2+Y/2+1);
				printf("->");				
				break;
			}

		case 3:
			{
				gotoxy(O1+X/2-6,O2+Y/2+2);
				printf("->");
				break;
			}

		case 4:
			{
				gotoxy(O1+X/2-6,O2+Y/2+3);
				printf("->");
				break;
			}

		default:break;
		}
	}

}

void menu_hard()//难度菜单
{
	int menu_hard_select();
	int selected=1;
	
	while(1)
	{
		menu_clean(1);
		gotoxy(O1+X/2-2,O2+Y/2);
		printf("1.入门");
		gotoxy(O1+X/2-2,O2+Y/2+1);
		printf("2.正常");
		gotoxy(O1+X/2-2,O2+Y/2+2);
		printf("3.挑战");
		gotoxy(O1+X/2-2,O2+Y/2+3);
		printf("4.返回");
		selected=menu_hard_select(selected);
		switch(selected)
		{

		case 1:
			{
				hard=1;
				gotoxy(O1+X/2-3,O2+Y/2);
				printf("*");
				break;
			}
		case 2:
			{
				hard=2;
				gotoxy(O1+X/2-3,O2+Y/2+1);
				printf("*");
				break;
			}
		case 3:
			{
				hard=3;
				gotoxy(O1+X/2-3,O2+Y/2+3);
				printf("*");
				break;
			}
		case 4:
			{
				menu_clean(1);
				return;
			}

		}
	}
	
}

int menu_hard_select(int selected)
{
	int keypressed;

	gotoxy(O1+X/2-6,O2+Y/2+selected-1);
	printf("->");
	gotoxy(O1+X/2-3,O2+Y/2+hard-1);
	printf("*");

	while(1)
	{
		gotoxy(0,0);
		keypressed=bioskey(1);

		switch(keypressed)
		{
		case UP:
			{
				if(selected==1)
				{
					selected=4;
					break;
				}
				else
					selected--;
				break;
			}
		case DOWN:
			{
				if(selected==4)
				{
					selected=1;
					break;
				}
				else
					selected++;
				break;
			}
		case ENTER:
			{
				gotoxy(O1+X/2-3,O2+Y/2+hard-1);
				printf(" ");
				return selected;
				break;
			}
		default:
			
				break;
			
		}
		
		menu_clean(0);

		switch(selected)
		{
		case 1:
			{

				gotoxy(O1+X/2-6,O2+Y/2);
				printf("->");
				break;
			}

		case 2:
			{

				gotoxy(O1+X/2-6,O2+Y/2+1);
				printf("->");				
				break;
			}

		case 3:
			{
				gotoxy(O1+X/2-6,O2+Y/2+2);
				printf("->");
				break;
			}

		case 4:
			{
				gotoxy(O1+X/2-6,O2+Y/2+3);
				printf("->");
				break;
			}

		default:break;
		}

			gotoxy(O1+X/2-3,O2+Y/2+hard-1);
			printf("*");

	}
}

void cleanall()
{
	int i,k;
	ZeroMemory(b,sizeof(b));
	ZeroMemory(a,sizeof(a));

	for(i=0;i<X;i++)
	{
		for(k=0;k<Y;k++)
		{
			gotoxy((i+O1),(k+O2-1));
			puts(" ");
		}
	}
	ZeroMemory(now,sizeof(now));
	ZeroMemory(next,sizeof(next));
	drawnext();
}

int record_save()				//记录分数
{
	FILE *fp;
	struct data *prep,*p;
	int i=0;
	prep=p=head;
	if((fp=fopen("rec.dat","wb"))==0)
	{
		MessageBox(NULL,"保存排行错误！","错误",0);
		return 0;
	}
	i=0;
	while(p)
	{
		prep=p;
		fwrite(prep,sizeof(struct data),1,fp);
		p=p->next;
		i++;
		if (i>9) break;
	}
	fclose(fp);
	return 1;
}

//////////////////////////文件读取
int record_read()
{
	FILE *fp;
	int i=0;

	if((fp=fopen("rec.dat","rb"))==0)
	{
		fp=fopen("rec.dat","w+b");
		if (!fp) 
		{
			MessageBox(NULL,"打开记录文件错误！","错误",0);
			return 0;
		}
	}
	
	for(i=0;i<10;i++)
	{
		if(!feof(fp))
		{
			fread(&rec[i],sizeof(struct data),1,fp);
			
		}
		rec[i].next=&rec[i+1];
		rec[i].no=i+1;
	}

	rec[9].next=0;
	rec[10].next=0;
	head=&rec[0];
	fclose(fp);
	return 1;

	
}

int record_edit()
{
	struct data *prep,*p;
	int i=1;
	
	prep=p=head;
	if(rec[10].score>rec[0].score )
	{
		head=&rec[10];
		head->next =prep;
	}
	else
	{
		for(i=1;i<10;i++)
		{
			if(rec[10].score>rec[i].score)
			{
				rec[i-1].next=&rec[10];
				rec[10].next=&rec[i];
				break;
			}
		}
	}
	i=0;
	prep=p=head;
	while(p)		//重新编号
	{
		prep=p;
		prep->no=i+1;
		p=p->next;
		i++;
		if(i>=9) break;
	}
	p->next=0;
	return 1;	
}
///////////////////////////////成绩记录
void record()
{
	int i=1;
	struct data *prep,*p;
	record_read();
	prep=p=head;

	record_read();
	if(final_score>rec[9].score )
	{	
			gotoxy(O1+X/2-7,O2+Y/2-1);
			printf("----------------");
			gotoxy(O1+X/2-3,O2+Y/2);
			printf(" 高 分 ！");
			gotoxy(O1+X/2-7,O2+Y/2+1);
			printf("姓名:      ");
			gotoxy(O1+X/2-7,O2+Y/2+2);
			printf("----------------");
			gotoxy(O1+X/2-2,O2+Y/2+1);
			for(i=0;i<6;i++)
			{
				scanf("%c",&rec[10].name[i]);
				if(rec[10].name[i]==12 || rec[10].name[i]==10)//过滤所有换行符并结束输入
				{
					rec[10].name[i]=0;
					break;
				}
			}
			rec[10].name[i]=0;
			//gets(rec[10].name);
			getch();
			rec[10].score=final_score;
			record_edit();
			record_save();
			cleanall();
			record_show();
			return;
	}
}
void record_show()
{
	int i=1;
	struct data *prep,*p;

	
	menu_clean(1);
	if(record_read())
	{
		gotoxy(O1+3,O2+5);
		printf("高手榜（TOP 10）");
		prep=p=head;
		gotoxy(O1+1,O2+7);
		printf("NO.    姓名   分数");
		while(p)
		{
			prep=p;
			p=p->next;
			gotoxy(O1+1,O2+7+i);
			printf("%-2d    %-6s %5ld",prep->no,prep->name,prep->score);
			i++;
			if(i>10) break;
		}
	}
	else
	{
		gotoxy(O1+2,O2+5);
		printf("读取信息错误！");
		//return;
	}

	getch();
	cleanall();
	return;




}

/////////////////////////按键函数
int keypress()
{
	int keypressed;
	keypressed=bioskey(0);
	switch(keypressed)
	{
				case 112:
			{
				gotoxy(O1+X+4,O2+13);
				printf("暂停");
				getch();
				gotoxy(O1+X+4,O2+13);
				printf("    ");

				return 0;
			}
		case 113:
			{
				return 1;
			}
	}
	return keypressed;
}
////////////////////////////////方块坐标////////////////////
void Iblock(void)
{
///////////////////////////////////////山形
////////////////左
block[0].x[0]=0;
block[0].x[1]=2;
block[0].x[2]=2;
block[0].x[3]=2;
block[0].y[0]=2;
block[0].y[1]=1;
block[0].y[2]=2;
block[0].y[3]=3;
//////////////////////////////////////下
block[1].x[0]=0;
block[1].x[1]=2;
block[1].x[2]=2;
block[1].x[3]=4;
block[1].y[0]=2;
block[1].y[1]=2;
block[1].y[2]=3;
block[1].y[3]=2;
/////////////////////////////////////右
block[2].x[0]=0;
block[2].x[1]=0;
block[2].x[2]=0;
block[2].x[3]=2;
block[2].y[0]=1;
block[2].y[1]=2;
block[2].y[2]=3;
block[2].y[3]=2;
///////////////////////////////////////上
block[3].x[0]=0;
block[3].x[1]=2;
block[3].x[2]=2;
block[3].x[3]=4;
block[3].y[0]=3;
block[3].y[1]=2;
block[3].y[2]=3;
block[3].y[3]=3;

//////////////////////////////////////////////////Z形1
//////////////////////////////////立
block[4].x[0]=0;
block[4].x[1]=0;
block[4].x[2]=2;
block[4].x[3]=2;
block[4].y[0]=2;
block[4].y[1]=3;
block[4].y[2]=1;
block[4].y[3]=2;
///////////////////////////////////平
block[5].x[0]=0;
block[5].x[1]=2;
block[5].x[2]=2;
block[5].x[3]=4;
block[5].y[0]=2;
block[5].y[1]=2;
block[5].y[2]=3;
block[5].y[3]=3;

////////////////////////////////////////Z形2
///////////////////////////////////////平
block[6].x[0]=0;
block[6].x[1]=0;
block[6].x[2]=2;
block[6].x[3]=2;
block[6].y[0]=1;
block[6].y[1]=2;
block[6].y[2]=2;
block[6].y[3]=3;
/////////////////////////////////////////////立
block[7].x[0]=0;
block[7].x[1]=2;
block[7].x[2]=2;
block[7].x[3]=4;
block[7].y[0]=3;
block[7].y[1]=2;
block[7].y[2]=3;
block[7].y[3]=2;

//////////////////////////////////L形1
block[8].x[0]=0;
block[8].x[1]=2;
block[8].x[2]=2;
block[8].x[3]=2;
block[8].y[0]=3;
block[8].y[1]=1;
block[8].y[2]=2;
block[8].y[3]=3;

block[9].x[0]=0;
block[9].x[1]=2;
block[9].x[2]=4;
block[9].x[3]=4;
block[9].y[0]=2;
block[9].y[1]=2;
block[9].y[2]=2;
block[9].y[3]=3;

block[10].x[0]=0;
block[10].x[1]=0;
block[10].x[2]=0;
block[10].x[3]=2;
block[10].y[0]=1;
block[10].y[1]=2;
block[10].y[2]=3;
block[10].y[3]=1;

block[11].x[0]=0;
block[11].x[1]=0;
block[11].x[2]=2;
block[11].x[3]=4;
block[11].y[0]=2;
block[11].y[1]=3;
block[11].y[2]=3;
block[11].y[3]=3;

//////////////////////////////////L形2
block[12].x[0]=0;
block[12].x[1]=2;
block[12].x[2]=2;
block[12].x[3]=2;
block[12].y[0]=1;
block[12].y[1]=1;
block[12].y[2]=2;
block[12].y[3]=3;

block[13].x[0]=0;
block[13].x[1]=0;
block[13].x[2]=2;
block[13].x[3]=4;
block[13].y[0]=2;
block[13].y[1]=3;
block[13].y[2]=2;
block[13].y[3]=2;

block[14].x[0]=0;
block[14].x[1]=0;
block[14].x[2]=0;
block[14].x[3]=2;
block[14].y[0]=1;
block[14].y[1]=2;
block[14].y[2]=3;
block[14].y[3]=3;

block[15].x[0]=0;
block[15].x[1]=2;
block[15].x[2]=4;
block[15].x[3]=4;
block[15].y[0]=3;
block[15].y[1]=3;
block[15].y[2]=2;
block[15].y[3]=3;

////////////////////////////////////////////////////////////方块
block[16].x[0]=0;
block[16].x[1]=0;
block[16].x[2]=2;
block[16].x[3]=2;
block[16].y[0]=2;
block[16].y[1]=3;
block[16].y[2]=2;
block[16].y[3]=3;

///////////////////////////////////////////长条
block[17].x[0]=0;
block[17].x[1]=2;
block[17].x[2]=4;
block[17].x[3]=6;
block[17].y[0]=3;
block[17].y[1]=3;
block[17].y[2]=3;
block[17].y[3]=3;

block[18].x[0]=0;
block[18].x[1]=0;
block[18].x[2]=0;
block[18].x[3]=0;
block[18].y[0]=0;
block[18].y[1]=1;
block[18].y[2]=2;
block[18].y[3]=3;
}
///////////////////////////////线程
DWORD WINAPI ThreadProc(
  LPVOID lpParameter   // thread data
)
{
	while(1)
	{
		randseed++;
		TIME++;
		Sleep(10);
	}
}
void encrypt(char *str)
{
	int i;
	int len;
	len=strlen(str);
	for(i=0;i<=len;i++)
		str[i]=str[i]^0xff;
}