/*
    Tetris:����˹����
    author:wangzequan
    Date:2023/01/04
*/
#include<stdlib.h>
#include <graphics.h>		// ����ͼ�ο�ͷ�ļ�
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
Block curBlock[4], nextBlock[4];     // ��ǰ�Ķ���˹����
/*
    ���в�ͬ������
*/
int blockType[7][4] = 
{   
    {0,1,5,6}, // z
    {1,2,4,5}, // s
    {2,4,5,6},// J
    {0,4,5,6},// L
    {1,4,5,6},// T
    {1,2,5,6},// ��
    {4,5,6,7} // I
};
int rotateCenter[7] = {1,0,2,2,2,0,1};  // ��ת����
int table[20][10];     // ����

int maxScore = 0;      // ��ߵ÷�
int curScore = 0;      // ��ǰ�÷�
int lines	 = 0;	   // ��������
int levels 	 = 1;      // ��ǰ����д��
int curType,nextType;
const double speedNormal = 0.5; // �����������ٶ�
const double speedQuick  = 0.015;// ���ٵ���Ϸ�ٶ�
bool isGaming   = false;        // �Ƿ�����Ϸ״̬
bool isQuitGame = false;        // �Ƿ��˳���Ϸ
bool isGameStart= true;         // �Ƿ�����Ϸ��ʼ��״̬
bool isBlockFalling = false;    // �жϷ����Ƿ�����½�
int deletedLines = 0;           // ��������������
// ����һ���������
int newType()
{
    return rand()%7;
}

// ��һ���µķ���
void newBlock(int index)
{
    for(int i=0;i<4;i++)
    {
        curBlock[i]    = nextBlock[i];     // ��Ԥ����ש�鸳�赱ǰ
        nextBlock[i].x = blockType[index][i]%4;
        nextBlock[i].y = blockType[index][i]/4;
    }
    return ;
}
// ��ʼ����ǰ����λ��
void initBlocks()
{
    curType = nextType;
    nextType = newType(); 
    newBlock(nextType);
    for(int i=0;i<4;i++)        // ����ǰ����ڷŵ���ʼλ������ȥ
    {
        curBlock[i].y-=2;
        curBlock[i].x+=3;
    }
    isBlockFalling = true;
}
// ������
void drawRec(int left,int top,int right, int bottom)
{  
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID); 
    setfillcolor(BLACK);
    rectangle(left,top,right, bottom);              // ��߿�
    solidrectangle(left + gap, top + gap, right - gap, bottom - gap);   //�����
}
// �ж�λ���Ƿ�Ϸ��ĺ���
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
    ������Ϸ����
    1. ����tableΪȫ0״̬
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
// ������еķ���,���Ҫ����������
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
        // �ж���Ϸ�Ƿ����
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
//������
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
// ����״̬
void update()
{
    for(int i=0;i<4;i++)                                               // �������������ͼ����ӵ�table��
    {
        if(curBlock[i].x>=0&&curBlock[i].x<10&&curBlock[i].y>=0&&curBlock[i].y<20
        && table[curBlock[i].y][curBlock[i].x]==0)
            table[curBlock[i].y][curBlock[i].x] = 1;
    }
    initBlocks();
    updateTable();
    return ;
}
// ������Ϸ����
void updateGameData()
{
    lines += deletedLines;
    curScore += isGaming?pow(2, deletedLines)*levels*10:0;
    maxScore = curScore >= maxScore?curScore:maxScore;
    return;
}
//��ת����
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

// �����ƶ�����
void moveLeft()
{
    for(int i=0;i<4;i++)
    {
        curBlock[i].x = curBlock[i].x - 1;
    }
    if(!check())    // ������Ϸ��ͻ�ԭ
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
    if(!check())    // ������Ϸ��ͻ�ԭ
    {
        for(int i=0;i<4;i++)
        {
            curBlock[i].x = curBlock[i].x - 1;
        }
    }
}

// ���亯��
void down()
{
    for(int i=0;i<4;i++)
    {
        curBlock[i].y = curBlock[i].y + 1;
    }
    if(!check())    // ������Ϸ��ͻ�ԭ,ͬʱ����״̬
    {
        for(int i=0;i<4;i++)
        {
            curBlock[i].y = curBlock[i].y - 1;
        }
        //����״̬
        isBlockFalling = false;
    }
}
// ׹�亯��(ֱ�����䵽�ײ�)
void fall()
{
    while(isBlockFalling)
    {
        down();
    }
    update();
    updateGameData();   // ������Ϸ����
    return;
}
// ��ʾ��ʾ��
void showGameGuide()
{
    settextcolor(GREEN);
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, _T("Consolas"));
	outtextxy(50,700,TEXT("���������ƶ�"));
	outtextxy(50,750,TEXT("���������ƶ�"));
	outtextxy(150,700,TEXT("���������ƶ�"));
	outtextxy(150,750,TEXT("������ת"));
    outtextxy(250,700,TEXT("D��ֱ�����"));
	outtextxy(250,750,TEXT("R�����¿�ʼ"));
	outtextxy(350,700,TEXT("E����ͣ"));
	outtextxy(350,750,TEXT("Q���˳���Ϸ"));
}
void drawBLocks(Block blocks[4],int offsetX,int offsetY)
{
    for(int i=0;i<4;i++)
    {
        if(blocks[i].x>=0&&blocks[i].x<10&&blocks[i].y>=0&&blocks[i].y<20)
        {
            // �����������
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
    outtextxy(400,50,TEXT("��߷�����"));
    sprintf(str,"%d", maxScore);
    outtextxy(400,110,(LPCTSTR)str);
    outtextxy(400,170,TEXT("��ǰ������"));
    sprintf(str,"%d", curScore);
    outtextxy(400,230,(LPCTSTR)str);
    outtextxy(400,290,TEXT("��ǰ����������"));
    sprintf(str,"%d", lines);
    outtextxy(400,350,(LPCTSTR)str);
    outtextxy(400,410,TEXT("��ǰ�ȼ���"));
    sprintf(str,"%d", levels);
    outtextxy(400,470,(LPCTSTR)str);
    outtextxy(400,530,TEXT("��һ��Ҫ���ֵ�ͼ�Σ�"));
    drawBLocks(nextBlock,400, 580);
}
/*
    ��ʾ��Ϸ����
*/
/*
void showGameArea()
{
    // ����߿�
    setlinecolor(BLACK);
    rectangle(25,25,350,675);
    // ����Ϸ������20x10�����߿򻭳���

}
*/
void showGameArea(int offsetX,int offsetY)
{
    // ����߿�
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID);
    rectangle(25,25,375,675);
    setlinestyle(PS_DASH);
    // ����Ϸ������20x10�����߿򻭳���
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<20;j++)
        {
            rectangle(offsetX+rectangleWidth*i,offsetY+rectangleWidth*j,offsetX+rectangleWidth*i+rectangleWidth,offsetY+rectangleWidth*j+rectangleWidth);
        }
    }  
}
//�ų���������
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
    initgraph(700, 800,EX_SHOWCONSOLE);	// ������ͼ���ڣ���СΪ 700x800 ����
    BeginBatchDraw();         // ������ͼ����
    nextType = newType();
    newBlock(nextType);
    start = clock();
    while(!isQuitGame)
    {
        cleardevice();
        IMAGE image;            // ����ͼƬ����
        loadimage(&image, TEXT("./image/background.jpg"),700,800);
        putimage(0, 0, &image);
        //std::cout<<isGameOver<<std::endl;
        if(isGaming&&isGameStart)   // ��ʼ��
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
                updateGameData();   // ������Ϸ����
            }
            delay = speedNormal;
        }
        /*
            �Ե�ǰ��ʽ��������
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
                delay = speedQuick;    // ��������
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
                // ��ԭ��������
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
            /*���������Ϸ״̬,��ų�һ����������*/
            showTetris();
        }
        showGameGuide();
        FlushBatchDraw();
    }
    EndBatchDraw();
	closegraph();			// �رջ�ͼ����
    return 0;
}