/*
    Tetris:俄罗斯方块
    author:wangzequan
    Date:2023/01/04
*/
#include<stdlib.h>
#include <graphics.h>		// 引用图形库头文件
#include <conio.h>
#include<winnt.h>
#include<stdio.h>
#include<time.h>
#include<iostream>
#include<math.h>
#define KEY_QUIT 81
#define KEY_quit 113
#define KEY_RESET 114
#define KEY_FALL 100
#define KEY_fall 68
#define KEY_reset 82
#define KEY_DOWN 80
#define KEY_UP  72
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_SPACE 32
#define rectangleWidth 30
#define gap 6

typedef struct block{
    int x,y;
}Block;
Block curBlock[4], nextBlock[4];     // 当前的俄罗斯方块
/*
    其中不同的类型
*/
int blockType[7][4] = 
{   
    {0,1,5,6}, // z
    {1,2,4,5}, // s
    {2,4,5,6},// J
    {0,4,5,6},// L
    {1,4,5,6},// T
    {1,2,5,6},// 田
    {4,5,6,7} // I
};
int rotateCenter[7] = {1,0,2,2,2,0,1};  // 旋转中心
int table[20][10];     // 背景

int maxScore = 0;      // 最高得分
int curScore = 0;      // 当前得分
int lines	 = 0;	   // 消除行数
int levels 	 = 1;      // 当前级别写死
int curType,nextType;
const double speedNormal = 0.5; // 正常的下落速度
const double speedQuick  = 0.015;// 快速的游戏速度
bool isGaming   = false;        // 是否处于游戏状态
bool isQuitGame = false;        // 是否退出游戏
bool isGameStart= true;         // 是否处于游戏开始的状态
bool isBlockFalling = false;    // 判断方块是否可以下降
int deletedLines = 0;           // 计算消除的行数
// 产生一个随机函数
int newType()
{
    return rand()%7;
}

// 画一个新的方块
void newBlock(int index)
{
    for(int i=0;i<4;i++)
    {
        curBlock[i]    = nextBlock[i];     // 将预备的砖块赋予当前
        nextBlock[i].x = blockType[index][i]%4;
        nextBlock[i].y = blockType[index][i]/4;
    }
    return ;
}
// 初始化当前方块位置
void initBlocks()
{
    curType = nextType;
    nextType = newType(); 
    newBlock(nextType);
    for(int i=0;i<4;i++)        // 将当前方块摆放到起始位置里面去
    {
        curBlock[i].y-=2;
        curBlock[i].x+=3;
    }
    isBlockFalling = true;
}
// 画方格
void drawRec(int left,int top,int right, int bottom)
{  
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID); 
    setfillcolor(BLACK);
    rectangle(left,top,right, bottom);              // 外边框
    solidrectangle(left + gap, top + gap, right - gap, bottom - gap);   //内填充
}
// 判断位置是否合法的函数
bool check()
{
    for(int i=0;i<4;i++)
    {
        if(curBlock[i].x<0||curBlock[i].x>9||curBlock[i].y>19
        || (curBlock[i].y>=0&&table[curBlock[i].y][curBlock[i].x]))
            return false;
    }
    return true;
}
/*
    重置游戏数据
    1. 更新table为全0状态
*/
void resetGameData()
{
    for(int i=0;i<20;i++)
    {
        for(int j=0;j<10;j++)
        {
            table[i][j] = 0;
        }
    }
    curScore = 0;
    lines = 0;
    deletedLines = 0;
}
// 画表格中的方格,如果要消行则消行
void updateTable()
{
    int k=19;
    deletedLines = 0;
    for(int i=19;i>=0;i--)
    {
        int cnt = 0;
        for(int j=9;j>=0;j--)
        {
            if(table[i][j])
                cnt++;
        }
        if(cnt!=10)
        {
            for(int j=9;j>=0;j--)
            {
                table[k][j] = table[i][j];
            }
            k--;
        }else{
            deletedLines++;
        }
        // 判断游戏是否结束
        if(k==-1&&!i&&cnt)
        {
            isGaming = false;
            resetGameData();
            return;
        }
    }
    while(k>=0)
    {   
        for(int j=9;j>=0;j--)
        {
            table[k][j] = 0;
        }
        k--;
    }
}
//画方格
void drawTable(int offsetX, int offsetY)
{
    for(int i=0;i<20;i++)
    {
        for(int j=0;j<10;j++)
        {
            if(table[i][j])
            {
                int top     = offsetX + i*rectangleWidth;
                int left    = offsetY + j*rectangleWidth;
                int bottom  = offsetX + i*rectangleWidth + rectangleWidth;
                int right   = offsetY + j*rectangleWidth + rectangleWidth;
                drawRec(left,top,right,bottom);
            }
        }
    }
}
// 更新状态
void update()
{
    for(int i=0;i<4;i++)                                               // 将不能再下落的图形添加到table中
    {
        if(curBlock[i].x>=0&&curBlock[i].x<10&&curBlock[i].y>=0&&curBlock[i].y<20
        && table[curBlock[i].y][curBlock[i].x]==0)
            table[curBlock[i].y][curBlock[i].x] = 1;
    }
    initBlocks();
    updateTable();
    return ;
}
// 更新游戏数据
void updateGameData()
{
    lines += deletedLines;
    curScore += isGaming?pow(2, deletedLines)*levels*10:0;
    maxScore = curScore >= maxScore?curScore:maxScore;
    return;
}
//旋转函数
void rotate(int index)
{
    if(index==5) return ;
    Block p = curBlock[rotateCenter[index]];
    for(int i=0;i<4;i++)
    {
        Block tmp = curBlock[i];
        curBlock[i].x = p.x + tmp.y - p.y;
        curBlock[i].y = p.y - tmp.x + p.x;
    }
}

// 左右移动函数
void moveLeft()
{
    for(int i=0;i<4;i++)
    {
        curBlock[i].x = curBlock[i].x - 1;
    }
    if(!check())    // 如果不合法就还原
    {
        for(int i=0;i<4;i++)
        {
            curBlock[i].x = curBlock[i].x + 1;
        }
    }
}

void moveRight()
{
    for(int i=0;i<4;i++)
    {
        curBlock[i].x = curBlock[i].x + 1;  
    }
    if(!check())    // 如果不合法就还原
    {
        for(int i=0;i<4;i++)
        {
            curBlock[i].x = curBlock[i].x - 1;
        }
    }
}

// 下落函数
void down()
{
    for(int i=0;i<4;i++)
    {
        curBlock[i].y = curBlock[i].y + 1;
    }
    if(!check())    // 如果不合法就还原,同时更新状态
    {
        for(int i=0;i<4;i++)
        {
            curBlock[i].y = curBlock[i].y - 1;
        }
        //更新状态
        isBlockFalling = false;
    }
}
// 坠落函数(直接下落到底部)
void fall()
{
    while(isBlockFalling)
    {
        down();
    }
    update();
    updateGameData();   // 更新游戏数据
    return;
}
// 显示提示语
void showGameGuide()
{
    settextcolor(GREEN);
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, _T("Consolas"));
	outtextxy(50,700,TEXT("←：向左移动"));
	outtextxy(50,750,TEXT("→：向右移动"));
	outtextxy(150,700,TEXT("↓：向下移动"));
	outtextxy(150,750,TEXT("↑：旋转"));
    outtextxy(250,700,TEXT("D：直接落底"));
	outtextxy(250,750,TEXT("R：重新开始"));
	outtextxy(350,700,TEXT("E：暂停"));
	outtextxy(350,750,TEXT("Q：退出游戏"));
}
void drawBLocks(Block blocks[4],int offsetX,int offsetY)
{
    for(int i=0;i<4;i++)
    {
        if(blocks[i].x>=0&&blocks[i].x<10&&blocks[i].y>=0&&blocks[i].y<20)
        {
            // 计算横纵坐标
            int left = offsetX +blocks[i].x*rectangleWidth;
            int top  = offsetY + blocks[i].y*rectangleWidth;
            int right = offsetX + blocks[i].x*rectangleWidth + rectangleWidth;
            int bottom = offsetY + blocks[i].y*rectangleWidth + rectangleWidth;
            drawRec(left,top,right,bottom);
        }
    }
}
void showGameData()
{
    settextcolor(RED);
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, _T("Consolas"));
    char str[100];
    memset(str, '/0',sizeof(str));
    outtextxy(400,50,TEXT("最高分数："));
    sprintf(str,"%d", maxScore);
    outtextxy(400,110,(LPCTSTR)str);
    outtextxy(400,170,TEXT("当前分数："));
    sprintf(str,"%d", curScore);
    outtextxy(400,230,(LPCTSTR)str);
    outtextxy(400,290,TEXT("当前消除行数："));
    sprintf(str,"%d", lines);
    outtextxy(400,350,(LPCTSTR)str);
    outtextxy(400,410,TEXT("当前等级："));
    sprintf(str,"%d", levels);
    outtextxy(400,470,(LPCTSTR)str);
    outtextxy(400,530,TEXT("下一个要出现的图形："));
    drawBLocks(nextBlock,400, 580);
}
/*
    显示游戏区域
*/
/*
void showGameArea()
{
    // 画外边框
    setlinecolor(BLACK);
    rectangle(25,25,350,675);
    // 将游戏区域用20x10的虚线框画出来

}
*/
void showGameArea(int offsetX,int offsetY)
{
    // 画外边框
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID);
    rectangle(25,25,375,675);
    setlinestyle(PS_DASH);
    // 将游戏区域用20x10的虚线框画出来
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<20;j++)
        {
            rectangle(offsetX+rectangleWidth*i,offsetY+rectangleWidth*j,offsetX+rectangleWidth*i+rectangleWidth,offsetY+rectangleWidth*j+rectangleWidth);
        }
    }  
}
//放出文字描述
void showTetris()
{
    setbkmode(OPAQUE);
    settextstyle(60, 0, _T("Consolas"));
    settextcolor(WHITE);
    RECT tetris = {100,220,300,420};
    drawtext(_T("TETRIS"), &tetris, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
int main()
{
    unsigned seed;
    seed = time(0);
    srand(seed);
    clock_t start, finish;
    double duration=0.00;
    double delay = speedNormal;
    initgraph(700, 800,EX_SHOWCONSOLE);	// 创建绘图窗口，大小为 700x800 像素
    BeginBatchDraw();         // 批量绘图操作
    nextType = newType();
    newBlock(nextType);
    start = clock();
    while(!isQuitGame)
    {
        cleardevice();
        IMAGE image;            // 定义图片对象
        loadimage(&image, TEXT("./image/background.jpg"),700,800);
        putimage(0, 0, &image);
        //std::cout<<isGameOver<<std::endl;
        if(isGaming&&isGameStart)   // 初始化
        {
            initBlocks();
            isGameStart = false;
        }
        finish = clock();
        duration = (double)(finish-start)/ CLOCKS_PER_SEC;
        if(isGaming&&duration>delay)
        {
            start = clock();
            down();
            if(isBlockFalling==false)
            {
                update();
                updateGameData();   // 更新游戏数据
            }
            delay = speedNormal;
        }
        /*
            对当前形式进行运算
        */
        if(_kbhit())
        {
            int key = _getch();
            printf("%d\n", key);
            if (key == KEY_UP&&isGaming)
            {
                rotate(curType);
            }
            else if(key == KEY_DOWN&&isGaming)
            {
                delay = speedQuick;    // 加速下落
            }
            else if(key == KEY_RIGHT&&isGaming)
            {
                moveRight();
            }
            else if(key == KEY_LEFT&&isGaming)
            {
                moveLeft();
            }
            else if((key == KEY_FALL||key == KEY_fall)&&isGaming)
            {
                fall();
            }
            else if(key == KEY_QUIT || key == KEY_quit)
            {
                isQuitGame = true;    
            }
            else if(key ==KEY_RESET || key == KEY_reset)
            {
                isGaming = !isGaming;
                // 还原比赛数据
                if(!isGaming)
                {
                    resetGameData();
                }
                else
                {
                    initBlocks();
                }
            }
            else if(key == KEY_SPACE&&isGaming)
            {
                system("pause");
            }
        }
        if(isGaming)
        {
            drawTable(50,50);
            drawBLocks(curBlock,50,50);
        }
        showGameArea(50,50);
        showGameData();
        if(!isGaming)
        {
            /*如果不是游戏状态,则放出一段文字描述*/
            showTetris();
        }
        showGameGuide();
        FlushBatchDraw();
    }
    EndBatchDraw();
	closegraph();			// 关闭绘图窗口
    return 0;
}