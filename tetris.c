#include<stdio.h>
#include<math.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include<conio.h>
#include<time.h>

#define TICK 1000
#define COOL 200
#define WIDE 12
#define HIGH 25
//定义的大小，非显示大小，为呈现下落效果x=wide=j=12  y=high=i=25

COORD fallera[3];
COORD next_fallera[3];
COORD fallerb[4];
COORD next_fallerb[4];
COORD fallerc[5];
COORD next_fallerc[5];

int pan;
int si=0;
int count=0;
char key;
int keyboard_flag;
char ditu[HIGH][WIDE];
//非显示的地图大小，显示为x:0-12 y:5-25
//空地图为0，已经下落为1，还正在移动的为2

HANDLE hdl;
ULONGLONG last,now;

//制作方块 
short shapea[2][6]=
{
	{6,1,6,2,7,2},
	{6,0,6,1,6,2}
};
short shapeb[7][8]=
{
	{6,1,7,1,6,2,7,2},
	{7,0,6,1,7,1,6,2},
	{6,0,6,1,7,1,7,2},
	{6,1,5,2,6,2,7,2},
	{5,1,5,2,6,2,7,2},
	{5,2,6,2,7,2,8,2},
	{7,1,7,2,6,2,5,2}
};
short shapec[3][10]=
{
	{6,0,6,1,6,2,7,2,8,2},
	{5,1,5,2,6,2,7,2,8,2},
	{5,2,6,2,7,2,8,2,8,1}
};

typedef enum zhuangtai
{
    kong, tang, dong
}zhuangtai;

typedef enum shapetype
{
    dshape,fshape,
	zshape,xshape,cshape,vshape,bshape,nshape,mshape,
	qshape,wshape,eshape
}shapetype;
shapetype nowshape;

void generate();
void update_screen();
void xiao();
int trydown();
int zhuan();
int zuoyou(int);
int jilu(int);
void dibu(int,int,int);
void sixunhuan();

int main()
{
	//int b=0;
	system("chcp 65001 && cls");
	
	out:
	count=0;
	hdl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci = { 1,0 };
    SetConsoleCursorInfo(hdl, &cci);
    SetConsoleTitle("TETRIS");
	//清除光标

    srand((unsigned)time(NULL));              
	//随机数

	memset(ditu, 0, WIDE * HIGH * sizeof(char));
	//初始化地图

	last=GetTickCount64();
    keyboard_flag=0;
	//开始计时

	system("cls");
	generate();
	update_screen();
	//生成方块第一个

	for (;;)
	{
		/*if (b==1)
		{
			last=last+GetTickCount64()-a;
			b=0;
		}*/
		
		
		//冷却
		if (keyboard_flag==1)
        {
            keyboard_flag = 0;
            last = GetTickCount64() + COOL;
            update_screen();
        }

		//自然下落
		now=GetTickCount64();
		if(last<now)
		{
			last=last+TICK;
			if(trydown()==0)
			{
				//根据不同形状做地图更新
				if(pan==3)
				{
					for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = tang;
				}
				if(pan==4)
				{
					for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = tang;
				}
				if(pan==5)
				{
					for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = tang;
				}
				xiao();
	            generate();
				{}
			}
			for(int i=0; i<= 12;i++)
			{
				if(ditu[4][i]==1)
				{
					printf("\nyou lose!");
					break;
				}
			}
			update_screen();
		}
		//按键检测
		while (_kbhit())
        {
            key = _getch();
            switch (key)
            {
				case 72:
				//上，即旋转
				if (zhuan()==1)keyboard_flag = 1;
				break;
				case 80:
				//下
				if (trydown()==1)keyboard_flag = 1;
				break;
				case 75:
				//左
				if (zuoyou(-1) ==1)keyboard_flag = 1;
				break;
				case 77:
				//右
				if (zuoyou(1) ==1)keyboard_flag = 1;
				break;
				case 32:
				//空格是暂停
				getch();
				system("pause");
				keyboard_flag = 1;
				//a=GetTickCount64();
				//b=1;
				break;
				case 'c': case'C':
				//刷新重玩
				goto out;
				break;

			}
		}
	}
	
	return 0;
}
void generate()
{
    nowshape = rand() % 12 ;
	if(nowshape<=1)
	{
		pan=3;
		memcpy(&fallera, shapea[nowshape], 3 * sizeof(COORD));
    for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = dong;
	}
	if(nowshape<=8&&nowshape>=2)
	{
		pan=4;
		memcpy(&fallerb, shapeb[nowshape-2], 4 * sizeof(COORD));
    for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = dong;
	}
	if(nowshape<=11&&nowshape>=9)
	{
		pan=5;
		memcpy(&fallerc, shapec[nowshape-9], 5 * sizeof(COORD));
    for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = dong;
	}
}
void update_screen()
{
    //system("CLS");
    COORD pos_start= { 0,0 };
    SetConsoleCursorPosition(hdl, pos_start);
	printf("Your score:%d",count);
	printf("\n操作指南：空格暂停/再连按两次任意键继续\n          方向← → ↓移动方块\n          按↑顺时针转动棋子\n          c/C重新开始游戏\n");
    for (int i = 4; i < HIGH; i++)
    {
        printf("<|");
        for (int j = 0; j < WIDE; j++)
        {
            if (ditu[i][j] == kong) printf(". ");
            else printf("■");
        }
        printf("|>\n");
    }
    printf("<-------------------------->\n");
	
}
int trydown()
{
	if(pan==3)
	{
	 memcpy(next_fallera, fallera, sizeof(fallera));
    for (int i = 0; i < 3; i++)
    {
        next_fallera[i].Y++;
        if (next_fallera[i].Y >= HIGH)return 0;
        if (ditu[next_fallera[i].Y][next_fallera[i].X] == tang)return 0;
    }
    for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = kong;
    memcpy(fallera, next_fallera, sizeof(fallera));
    for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = dong;
    return 1;
	}
	if(pan==4)
	{
	 memcpy(next_fallerb, fallerb, sizeof(fallerb));
    for (int i = 0; i < 4; i++)
    {
        next_fallerb[i].Y++;
        if (next_fallerb[i].Y >= HIGH)return 0;
        if (ditu[next_fallerb[i].Y][next_fallerb[i].X] == tang)return 0;
    }
    for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = kong;
    memcpy(fallerb, next_fallerb, sizeof(fallerb));
    for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = dong;
    return 1;
	}
	if(pan==5)
	{
	 memcpy(next_fallerc, fallerc, sizeof(fallerc));
    for (int i = 0; i < 5; i++)
    {
        next_fallerc[i].Y++;
        if (next_fallerc[i].Y >= HIGH)return 0;
        if (ditu[next_fallerc[i].Y][next_fallerc[i].X] == tang)return 0;
    }
    for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = kong;
    memcpy(fallerc, next_fallerc, sizeof(fallerc));
    for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = dong;
    return 1;
	}
}
void xiao()
{
	COORD temp_ditu[HIGH][WIDE];
    int t;
    for (int i = HIGH - 1; i >= 3; i--)
    {
        t= 1;
        for (int j = 0; j < WIDE; j++)
        {
            if (ditu[i][j] != tang)
            {
                t= 0;
                break;
            }
        }
        if (t==1)
        {
			count++;
            memcpy(temp_ditu, ditu, i * sizeof(ditu[0]));
            memcpy(ditu + 1, temp_ditu, i * sizeof(ditu[0]));
            continue;
        }
    }
}
int zuoyou(int t)
{
	//左移t=-1;右移t=1
	if(pan==3)
	{
	memcpy(next_fallera, fallera, sizeof(fallera));
    for (int i = 0; i < 3; i++)
    {
        next_fallera[i].X+=t;
        if (next_fallera[i].X >= WIDE || next_fallera[i].X < 0) return 0;
        if (ditu[next_fallera[i].Y][next_fallera[i].X] == 1)return 0;
    }
    keyboard_flag = 1;
    for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = 0;
    memcpy(fallera, next_fallera, sizeof(fallera));
    for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = 2;
    return 1;
	}
	if(pan==4)
	{
	memcpy(next_fallerb, fallerb, sizeof(fallerb));
    for (int i = 0; i < 4; i++)
    {
        next_fallerb[i].X+=t;
        if (next_fallerb[i].X >= WIDE || next_fallerb[i].X < 0) return 0;
        if (ditu[next_fallerb[i].Y][next_fallerb[i].X] == 1)return 0;
    }
    keyboard_flag = 1;
    for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = 0;
    memcpy(fallerb, next_fallerb, sizeof(fallerb));
    for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = 2;
    return 1;
	}
	if(pan==5)
	{
	memcpy(next_fallerc, fallerc, sizeof(fallerc));
    for (int i = 0; i < 5; i++)
    {
        next_fallerc[i].X+=t;
        if (next_fallerc[i].X >= WIDE || next_fallerc[i].X < 0) return 0;
        if (ditu[next_fallerc[i].Y][next_fallerc[i].X] == 1)return 0;
    }
    keyboard_flag = 1;
    for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = 0;
    memcpy(fallerc, next_fallerc, sizeof(fallerc));
    for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = 2;
    return 1;
	}
}
int zhuan()
{
	if (nowshape == 2)return 0;
    if (pan==3)
	{
    for (int round = 0; round <= 1; round++)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                next_fallera[j].X = fallera[i].X + 1 * (fallera[i].Y - fallera[j].Y);        
                next_fallera[j].Y = fallera[i].Y + 1 * (fallera[j].X - fallera[i].X) + round;
            }   
			int t=1;
			for (int i = 0; i < 3; i++)
    		{
        		if (next_fallera[i].X < 0 || next_fallera[i].X >= WIDE)t=0;
        		if (next_fallera[i].Y >= HIGH)t= 0;
    		}
    		for (int i = 0; i < 3; i++)
    		{
        		if (ditu[next_fallera[i].Y][next_fallera[i].X] == 1)t=0;
   			}
            if (t==1)
            {
                for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = 0;
				int x=jilu(3);
                memcpy(fallera, next_fallera, sizeof(fallera));
				int y=jilu(3);
				dibu(x,y,3);
                for (int i = 0; i < 3; i++) ditu[fallera[i].Y][fallera[i].X] = 2;
                return 1;
            }
        }
    }
    return 0;
	}
	if (pan==4)
	{
    for (int round = 0; round <= 1; round++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                next_fallerb[j].X = fallerb[i].X + 1 * (fallerb[i].Y - fallerb[j].Y);
                next_fallerb[j].Y = fallerb[i].Y + 1 * (fallerb[j].X - fallerb[i].X) + round;
            }
			int t=1;
			for (int i = 0; i < 4; i++)
    		{
        		if (next_fallerb[i].X < 0 || next_fallerb[i].X >= WIDE)t=0;
        		if (next_fallerb[i].Y >= HIGH)t= 0;
    		}
    		for (int i = 0; i < 4; i++)
    		{
        		if (ditu[next_fallerb[i].Y][next_fallerb[i].X] == 1)t=0;
   			}
            if (t==1)
            {
                for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = 0;
				int a=jilu(4);
                memcpy(fallerb, next_fallerb, sizeof(fallerb));
				int b=jilu(4);
				dibu(a,b,4);
                for (int i = 0; i < 4; i++) ditu[fallerb[i].Y][fallerb[i].X] = 2;
                return 1;
            }
        }
    }
    return 0;
	}
	if (pan==5)
	{
    for (int round = 0; round <= 1; round++)
    {
        for (int i = 0; i <5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                next_fallerc[j].X = fallerc[i].X + 1 * (fallerc[i].Y - fallerc[j].Y);
                next_fallerc[j].Y = fallerc[i].Y + 1 * (fallerc[j].X - fallerc[i].X) + round;
            }
			int t=1;
			for (int i = 0; i < 5; i++)
    		{
        		if (next_fallerc[i].X < 0 || next_fallerc[i].X >= WIDE)t=0;
        		if (next_fallerc[i].Y >= HIGH)t= 0;
    		}
    		for (int i = 0; i < 5; i++)
    		{
        		if (ditu[next_fallerc[i].Y][next_fallerc[i].X] == 1)t=0;
   			}
            if (t==1)
            {
                for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = 0;
				int a=jilu(5);
                memcpy(fallerc, next_fallerc, sizeof(fallerc));
				int b=jilu(5);
				dibu(a,b,5);
                for (int i = 0; i < 5; i++) ditu[fallerc[i].Y][fallerc[i].X] = 2;
                return 1;
            }
        }
    }
    return 0;
	}
}
int jilu(int x)
{
	int min;
	if(x==3)
	{
		min=HIGH-fallera[0].Y;
		for (int i = 0; i < 3; i++)
		{
			if(min>HIGH-fallera[i].Y)
			min=HIGH-fallera[i].Y;
		}
		return min;
	}
	if(x==4)
	{
		min=HIGH-fallerb[0].Y;
		for (int i = 0; i < 4; i++)
		{
			if(min>HIGH-fallerb[i].Y)
			min=HIGH-fallerb[i].Y;
		}
		return min;
	}
	if(x==5)
	{
		min=HIGH-fallerc[0].Y;
		for (int i = 0; i < x; i++)
		{
			if(min>HIGH-fallerc[i].Y)
			min=HIGH-fallerc[i].Y;
		}
		return min;
	}
}
void dibu(int a,int b,int c)
{
	if(c==3)
	{
		for(int i;i<c;i++)
		{
			fallera[i].Y=fallera[i].Y-(a-b);
		}
	}
	if(c==4)
	{
		for(int i;i<c;i++)
		{
			fallerb[i].Y=fallerb[i].Y-(a-b);
		}
	}
	if(c==5)
	{
		for(int i;i<c;i++)
		{
			fallerc[i].Y=fallerc[i].Y-(a-b);
		}
	}
}
//代码有相关视频的学习，也有在csdn上部分函数用法的搜索，但是此项目确实由学习后独立完成，感谢各大网站与平台！