#include <stdio.h>
#include <easyx.h>
#include <conio.h>

#define WIDTH 422
#define HEIGHT 750
#define HERO_WIDTH 66
#define HERO_HEIGHT 82
#define BULLET_MAX 20
#define ENEMY_NUM 10
#define ENEMY_WIDTH 34
#define ENEMY_HEIGHT 26
#pragma warning(disable:4996)
#pragma warning(disable:6031)

IMAGE imgHeroMask;
IMAGE imgHero;
IMAGE imgBackground;
IMAGE imgBulletMask;
IMAGE imgBullet;
IMAGE imgEnemy;
IMAGE imgEnemyMask;
DWORD t1, t2, t3, t4;

void putTransparentImage(int x, int y, const IMAGE* mask, const IMAGE* img)//透明图片函数
{
    putimage(x, y, mask, SRCAND);
    putimage(x, y, img, SRCPAINT);
}

struct plane//构造飞机、子弹、敌机的结构体
{
    int x;
    int y;
    bool live;
    int score;
    int hp;
}player,bullet[BULLET_MAX],enemy[ENEMY_NUM];

void gameInit()//初始化
{
    t1 = t2 =t3=t4= GetTickCount();
    player.x = WIDTH /2;
    player.y = HEIGHT - 90;
    player.live = true;
    player.hp = 3;
    player.score = 0;
    for (int i = 0; i < BULLET_MAX; i++)
    {
        bullet[i].live = false;
    }
    for (int i = 0; i < ENEMY_NUM; i++)
    {
        enemy[i].live = false;
        enemy[i].hp = 1;
    }
}
void drawbackground()//画背景
{
    loadimage(&imgBackground, "./background.png");
    putimage(0, 0, &imgBackground);
}

void draw()//加载图片
{
    loadimage(&imgBackground, "./background.png");
    loadimage(&imgHeroMask, "./hero./hero0_mask.png");
    loadimage(&imgHero, "./hero./hero0.png");
    loadimage(&imgBulletMask, "./bullet./bullet_mask.png");
    loadimage(&imgBullet, "./bullet./bullet.png");
    loadimage(&imgEnemyMask, "./enemy./enemy0_mask.png");
    loadimage(&imgEnemy, "./enemy./enemy0.png");
    putimage(0, 0, &imgBackground);
    putTransparentImage(player.x, player.y, &imgHeroMask, &imgHero);
    for (int i = 0; i < BULLET_MAX; i++)
    {
        if (bullet[i].live == true)
        {
            putTransparentImage(bullet[i].x, bullet[i].y, &imgBulletMask, &imgBullet);
        }
    }
    for (int i = 0; i < ENEMY_NUM; i++)
    {
        if (enemy[i].live == true)
        {
            putTransparentImage(enemy[i].x, enemy[i].y, &imgEnemyMask, &imgEnemy);
        }
    }
    
}

void fire()//初始化子弹
{
    for (int i = 0; i < BULLET_MAX; i++)
    {
        if (bullet[i].live == false)
        {
            bullet[i].live = true;
            bullet[i].x = player.x+HERO_WIDTH/2;
            bullet[i].y = player.y;
            break;
        }
    }
}

void CreatEnemy()//初始化敌机
{
    for (int i = 0; i < ENEMY_NUM; i++)
    {
        if (enemy[i].live == false)
        {
            enemy[i].live = true;
            enemy[i].hp = 1;
            enemy[i].x = rand() % (WIDTH - ENEMY_WIDTH);
            enemy[i].y = 0;
            break;
        }
    }
}

void EnemyMove(int speed)//判断敌机是否存在，并让敌机动起来
{
    for (int i = 0; i < ENEMY_NUM; i++)
    {
        if (enemy[i].live == true)
        {

            enemy[i].y += speed;
            if (enemy[i].y >= HEIGHT)
            {
                enemy[i].live = false;
            }
        }
    }
}

void BulletMove(int speed)//改变子弹的位置，并定义子弹的移动速度
{
    for (int i = 0; i < BULLET_MAX; i++)
    {
        if (bullet[i].live == true)
        {
            bullet[i].y -= speed;
            if (bullet[i].y <= 0)
            {
                bullet[i].live = false;
            }
        }
    }
}

void write()//存档，存游戏中的玩家血量和玩家成绩的数据，按“e”实现。
{
    FILE* fp;
    fp = fopen("sourcehomeworkin.txt", "w");
    fprintf(fp, "%d %d", player.hp, player.score);
    fclose(fp);
}

void read()//读档，读档游戏中的玩家血量和玩家成绩的数据，按“r”实现。
{
    FILE* fp;
    fp = fopen("sourcehomeworkin.txt", "r");
    fscanf(fp, "%d %d", &player.hp, &player.score);
    fclose(fp);
}

void move(int speed)//使飞机动起来，并接收键盘输入。wasd移动，空格发射子弹,e存档，r读档
{
    if (_kbhit() != 0) 
    {
        char c = _getch();
        switch (c)
        {
        case'w':
            if(player.y>0)
            {
                player.y -= speed;
            }
            break;
        case's':
            if(player.y<HEIGHT-HERO_HEIGHT)
            {
                player.y += speed;
            }
            break;
        case'a':
            if (player.x > 0)
            {
                player.x -= speed;
            }
            break;
        case'd':
            if (player.x < WIDTH - HERO_WIDTH)
            {
                player.x += speed;
            }            
            break;
        case'e':
            write();
            player.hp = 0;
            break;
        case'r':
            read();
            break;
        case' ':
            if (t2 - t1 > 100) 
            {
                fire();
                t1 = t2;
            }
            t2= GetTickCount();

            BulletMove(1);

        }
    }
    if (t3 - t4 > 1000)
    {
        CreatEnemy();
        t3 = t4;
    }
    t3 = GetTickCount();
    EnemyMove(1);
}

void boom()//判断子弹和敌机的位置，如果重叠，子弹和敌机都消失
{
    for (int i = 0; i < ENEMY_NUM; i++)
    {
        if (!enemy[i].live)
            continue;
        for (int j = 0; j < BULLET_MAX; j++)
            {
                if (!bullet[j].live)
                    continue;
                if (enemy[i].x <= bullet[j].x && bullet[j].x <= enemy[i].x + ENEMY_WIDTH 
                    && enemy[i].y <= bullet[j].y && bullet[j].y <= enemy[i].y)
                    {
                        enemy[i].hp = enemy[i].hp - 1;
                        bullet[j].live = false;
                        player.score = player.score + 10;                        
                    }
                
            }
            if (enemy[i].hp <= 0)
                enemy[i].live = false;
     
    }
}

void hurt()//判断敌机和玩家飞机的位置，如果重合，玩家hp-1
{
    for (int i=0; i < ENEMY_NUM; i++)
    {
        if (!enemy[i].live)
            continue;

        if (player.x <= enemy[i].x && enemy[i].x <= player.x + HERO_WIDTH &&
            player.y <= enemy[i].y + ENEMY_HEIGHT && player.y + HERO_HEIGHT <= enemy[i].y
            )
        {
            player.hp = player.hp - 1;
            enemy[i].live = false;
        }
    }
    if (player.hp <= 0)
        player.live = false;
}

void playmusic()//播放背景音乐
{
    mciSendString("open ./music./background.wma", NULL, 0, NULL);
    mciSendString("play ./music./background.wma repeat", NULL, 0, NULL);
}

void stopmusic()
{
    mciSendString("stop ./music./background.wma", NULL, 0, NULL);
    mciSendString("close ./music./background.wma ", NULL, 0, NULL);
}

int main() //主函数
{
    initgraph(WIDTH,HEIGHT,EX_SHOWCONSOLE);
    setbkcolor(WHITE);
    cleardevice();
    drawbackground();
    gameInit();
    BeginBatchDraw();
    playmusic();
    while (player.hp>0)
    {
        BulletMove(3);
        move(10);
        draw();
        boom();
        FlushBatchDraw();
        cleardevice();
        hurt();
    }
    EndBatchDraw();
    stopmusic();
    setbkcolor(YELLOW);
    cleardevice();
    settextcolor(BLACK);
    settextstyle(50, 0, "微软雅黑");
    const char* pText = "GAME OVER";
    outtextxy(30, 300, pText);
    printf("您的成绩\n");
    printf("%d\n", player.score);
    if (player.score != 0)
    {
        FILE* fp;
        fp = fopen("sourcehomeworkscore.txt", "a");
        fprintf(fp, " %d ", player.score);
        fclose(fp);
        player.score = 0;
    }
    int score[5];
    for (int a = 0; a < 4; a++)
    {
        score[a] = 0;
    }
    FILE* fp;
    fp = fopen("sourcehomeworkscore.txt", "r");
    fscanf(fp, "%d", &score[4]);
    int i = 0;
    while (!feof(fp))
    {
        score[i] = player.score;
        fscanf(fp, "%d", &score[i]);
        i++;
    }
    int temp = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4 - i; j++)
        {
            if (score[j] < score[j + 1])
            {
                temp = score[j];
                score[j] = score[j + 1];
                score[j + 1] = temp;
            }
        }
    }
    fclose(fp);
    printf("成绩排行榜\n");
    for (int a=0; a < 5; a++)
    {
        printf("%d\n", score[a]);
    }
    
    getchar();
    return 0;
}