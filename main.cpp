#include "data.h"

extern Mat mapgraphic;
extern Mat HEAD;
extern Mat HEADNOW;
extern int gamemode;
extern int timecounter; //시스템 시간 흐름
extern int field[MAPSIZE][MAPSIZE]; //맵
extern int snakelong[MAPSIZE * MAPSIZE][6]; //rows - 뱀 최대길이, cols - 몸통 마디마다 x와 y좌표, rows가 0이면 마디없고 1이면 마디있음
extern int speed; // 뱀이동속도1~10단계로 1에서 10배속 1000 ~ 100
extern int score;// 쥐 잡아먹은 마리수
extern int direction;
extern int block;
extern int key;
extern int rocknum;

extern void getkey(); //waitKey대용
extern void graphic(); //그래픽처리
extern void logographic(); //시작시 추가 그래픽
extern void mice(); //땃쥐 랜덤 생성
extern void ROCK(); //장애물 랜덤 생성
extern void fieldcheck();
extern void snakebody(); //뱀 길이 및 좌표
extern void snakemove();
extern void mouse_callback(int event, int x, int y, int flags, void* userdata); //마우스 클릭 이벤트
extern void gameover();
int main()
{
    while (1)
    {
        if (gamemode == 0)
        {
            score = 0;
            rocknum = 0;
            direction = 3;
            fill(&field[0][0], &field[MAPSIZE - 1][MAPSIZE - 1], 0);
            HEADNOW = HEAD;
            snakelong[0][1] = 9;
            snakelong[0][2] = 9;
            snakelong[1][1] = 8;
            snakelong[1][2] = 9;
            snakelong[2][1] = 7;
            snakelong[2][2] = 9;
            snakelong[3][1] = 6;
            snakelong[3][2] = 9;
            fill(&snakelong[4][0], &snakelong[MAPSIZE * MAPSIZE - 1][5], -1);
            field[13][9] = 1;
            gamemode = 1;
        }
        while (gamemode == 1)
        {
            graphic();
            logographic();
            getkey();
            if (key == 27)
                return 0;
        }

        while (gamemode == 2)
        {
            getkey();
            if (key == 27)
                return 0;
            snakemove();
            if (key == 'D') //Dead or score 396(end)
            {
                gamemode = 3;
                break;
            }
            snakebody();
            if (score + 4 + rocknum == MAPSIZE * MAPSIZE)
            {
                gamemode = 3;
                break;
            }
            fieldcheck();
            ROCK();
            mice();
            graphic();
        }


        while (gamemode == 3)
        {
            gameover();
            key = waitKey(0);
            if (key == 27 || key == 'r' || key == 'R')
                break;
        }
        if (key == 27)
            break;
        else if (key == 'r' || key == 'R')
        {
            gamemode = 0;
            continue;
        }
    }
    destroyAllWindows();
}