#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include<time.h>
#include <conio.h>
#include <io.h>
static int g_nScreenIndex;
static HANDLE g_hScreen[2];
int x, y;
//화면 버퍼 초기화
void ScreenInit()
{
    CONSOLE_CURSOR_INFO cci;

    // 화면 버퍼 2개를 만든다.
    g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);//생성
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    // 커서를 숨긴다.
    cci.dwSize = 1;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(g_hScreen[0], &cci);//커서 설정을 위에 선언한 cci로
    SetConsoleCursorInfo(g_hScreen[1], &cci);
}

//활성화된 버퍼와 비활성화 버퍼 교체
void ScreenFlipping()
{
    SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
    g_nScreenIndex = ~g_nScreenIndex;//교환
}

//버퍼가 화면에 표시되는 동안 비활성화된 버퍼를 지우는 함수
void ScreenClear()
{
    //좌표
    COORD Coor = { 0, 0 };
    //cpu가 한 번에 처리할 수 있는 데이터의 크기 단위 WORD
    //DWORD(unsigned long), bool(char), BOOL(int), BYTE(unsigned char), WORD(unsigned short), UINT(unsigned int)
    DWORD dw;
    //(원하는 화면, 원하는 문자(' '), 원하는 넓이, 원하는 커서, 넓이를 반환할 곳)
    FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', 100 * 50, Coor, &dw);
}

//(x, y)좌표에 string 출력
void ScreenPrint(int x, int y, char* string)
{
    DWORD dw;
    COORD CursorPosition = { x, y };
    SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
    WriteFile(g_hScreen[g_nScreenIndex], string, strlen(string), &dw, NULL);
}

//화면 없앰
void ScreenRelease()
{
    CloseHandle(g_hScreen[0]);
    CloseHandle(g_hScreen[1]);
}
struct {
    bool exist;
    int x, y;
}bb[100];
char bbs[2] = "0";

typedef struct Boss1 {
    int x, y;
    int life;
}boss;

//적 총알
struct {
    int x, y;
    int gotoo;//어느쪽으로 갈지
    int type = 0;//어떤 총알인지
    bool shot;//쐈는지 안쐈는지
}enermy_bullet[100];
//적
struct {
    int x, y;
    int whereexist = 5;//4방향중 어느쪽에 있는지(5는 없는값.)
    int type = 0;//어떤 적인지(1은 일직선 총)
}enermy[30];//
struct {
    bool hitxy[100][100];
    int type;
}weapon;
void gob1(int i) {
    if (enermy[i].whereexist == 0) {
        enermy_bullet[i].y += 1;
    }
    else if (enermy[i].whereexist == 1) {
        enermy_bullet[i].x++;
    }
    else if (enermy[i].whereexist == 2) {
        enermy_bullet[i].x--;
    }
    else if (enermy[i].whereexist == 3) {
        enermy_bullet[i].y--;
    }
}

FILE* fp;
int gold;
int highscore;
int weaponsave;
int healp1;
int healp2;
int healp3;
int mclock;
void save() {
    fprintf(fp, "%d\n%d\n%d\n%d\n%d\n%d\n%d", gold, highscore, weaponsave, healp1, healp2, healp3, mclock);
    fclose(fp);
}
void gotoxy(int x, int y);
void drawaru(int y) {
    char gametitle[10][1][50] =
    {
{"              .     .^~~!!!!!~."},
{"  77!!!^     ^J             .~5"},
{":7?   :!J:   5^     .:!!!!!!!7Y"},
{"Y^      :5. ??      .G!"},
{"P.     .:5 P!!!.    ^~~!!!!!!~."},
{"^?~^~!7!~: Y^"},
{"          !J      ^^.         ..~^"},
{"          5^      ^~!!!!!??!!!!!~:"},
{"         ^Y              "},
{"         .               "},
    };
    int titley = y;
    for (int y = 0; y < 40; y++) {
        for (int x = 0; x < 70; x++) {
            gotoxy(x, y);
            if (y == 0 || y == 39) {
                printf("-");
            }
            else if (x == 0 || x == 69) {
                printf("|");
            }
        }
    }
    for (int i = 0; i < 10; i++) {
        gotoxy(20, titley);
        titley += 1;
        for (int o = 0; o < 50; o++) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            printf("%c", gametitle[i - 1][1][o]);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
        }
    }
}
bool opengate(int(*maplist)[4], int* mapplayerx, int* mapplayery, int* playerx, int* playery,int *crr);
int menuscreen();
bool hit(int playerx, int playery);

int main() {//더블버퍼링 없는 메인화면만들때 커서인포 설정해야할수도있음

    char sgold[10];

    char shighscore[10];

    char sweaponsave[10];

    char shealp1[10];
    char shealp2[10];
    char shealp3[10];
    char smclock[10];
    if (_access("savefile.txt", 0) != -1) {
        fopen_s(&fp, "savefile.txt", "rt");
        fgets(sgold, 10, fp);
        gold = atoi(sgold);
        fgets(shighscore, 10, fp);
        highscore = atoi(shighscore);
        fgets(sweaponsave, 10, fp);
        weaponsave = atoi(sweaponsave);
        fgets(shealp1, 10, fp);
        healp1 = atoi(shealp1);
        fgets(shealp2, 10, fp);
        healp2 = atoi(shealp2);
        fgets(shealp3, 10, fp);
        healp3 = atoi(shealp3);
        fgets(smclock, 10, fp);
        mclock = atoi(smclock);

        fclose(fp);
    }

    fopen_s(&fp, "savefile.txt", "wt");
    atexit(save);
    int round = 1;
    HANDLE hstd = GetStdHandle(STD_OUTPUT_HANDLE);
    srand(time(NULL));
    int count = 0;
    int playerx = 30;
    int playery = 10;
    char player[4] = "■";
    char enermyp[2] = "@";
    char b11[2] = "|";
    char b12[3] = "--";
    system("mode con cols=70 lines=40");
    system("title 아르");
    int score = 0;
    int life = 30;
    int weaponcool = 0;
    int lifecool = 0;
    int clockcool = 0;
    char weaponskin1[4] = "■";
    char ascore[10000];
    bool use_weapon = false;
    char ypan[2] = "|";
    char xpan[2] = "-";
    int menu = 0;
    int con2 = 0;
    int maplist[4][4] = { 0 };
    int maplisty = 0;
    int maplistx = 0;
    int rand3 = 0;
    int levelecon = 0;
    int levelemax = 30;
    int brokenecon = 0;
    int mapplayerx = 0;
    int mapplayery = 0;
    int maxemap[4][4];
    int crr = 0;
    for (int i = 0; i < 4; i++) {
        for (int o = 0; o < 4; o++) {
            maxemap[i][o] = 5;
        }
    }
    int crea = 0;
    bool end = false;
    //struct {
    //    struct {

    //    };
    //    char barrier1[2][5][4] = {
    //        {"■","■","■","■","■"},
    //        {"■","■","■","■","■"}
    //    };
    //    int barrier1xy[2] = { 10,5 };
    //};
    while (1) {
        life = 30;
        playerx = 30;
        playery = 10;
        for (int i = 0; i < 100; i++) {
            enermy[i].type = 0;
        }
        system("cls");
        menu = menuscreen();
        system("cls");
        if (menu == -1) {
            return 0;
        }
        if (menu == 2)
        {
            system("cls");

            int whereiscurser = 20;

            drawaru(3);
            gotoxy(5, 15);
            printf("사용아이템은 판매 번호를 누르면 사용됩니다.(체력물약은 1번)");
            gotoxy(28, whereiscurser);
            printf("-");
            while (1) {
                if (((GetAsyncKeyState(VK_ESCAPE) & 1) != 0)) {
                    system("cls");
                    gotoxy(28, 10);
                    printf("로딩 중입니다.");
                    break;
                }
                gotoxy(2, 2);
                printf("보유 골드:%d", gold);
                gotoxy(2, 28);
                printf("보유아이템.");
                gotoxy(2, 29);
                printf("체력물약 %d개", healp1);
                gotoxy(2, 30);
                printf("체력물통 %d개", healp2);
                gotoxy(2, 31);
                printf("체력물탱크 %d개", healp3);
                gotoxy(2, 32);
                printf("모래시계 %d개", mclock);
                gotoxy(30, 20);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
                printf("1. 체력물약(체력 10 회복)    $20");
                gotoxy(30, 22);
                printf("2. 체력물통(체력 20 회복)    $38");
                gotoxy(30, 24);
                printf("3. 체력물탱크(체력 40 회복)  $60");
                gotoxy(30, 26);
                printf("4. 모래시계                  $60");
                gotoxy(30, 28);
                printf("십자 무기                    $200");
                if (((GetAsyncKeyState(VK_SPACE) & 1) != 0) || ((GetAsyncKeyState(VK_RETURN) & 1) != 0)) {
                    if (whereiscurser == 20 && gold >= 20) {
                        gold -= 20;
                        healp1 += 1;
                    }
                    if (whereiscurser == 22 && gold >= 38) {
                        healp2 += 1;
                        gold -= 38;
                    }
                    if (whereiscurser == 24 && gold >= 60) {
                        gold -= 60;
                        healp3++;
                    }
                    if (whereiscurser == 26 && gold >= 60) {
                        gold -= 60;
                        mclock++;
                    }
                    if (whereiscurser == 28 && gold >= 200) {
                        gold -= 200;
                        weaponsave = 1;
                    }
                }
                if (((GetAsyncKeyState(VK_DOWN) & 1) != 0)) {
                    gotoxy(28, whereiscurser);
                    printf("  ");
                    if (whereiscurser < 28) {
                        whereiscurser += 2;
                        gotoxy(28, whereiscurser);
                    }
                    else {
                        gotoxy(28, 20);
                        whereiscurser = 20;
                    }
                    printf("-");
                }
                if (((GetAsyncKeyState(VK_UP) & 1) != 0)) {
                    gotoxy(28, whereiscurser);
                    printf("  ");
                    if (whereiscurser > 20) {
                        whereiscurser -= 2;
                        gotoxy(28, whereiscurser);
                    }
                    else {
                        gotoxy(28, 28);
                        whereiscurser = 28;
                    }
                    printf("-");
                }
            }
            Sleep(2000);
        }
        if (menu == 3) {
            return 0;
        }
        if (menu == 1) {
            round = 1;
            weapon.type = weaponsave;
            for (int go = 0; go < 3; go++) {
                crr = 0;
                if (end == true) {
                    break;
                }
                mapplayerx = 0;
                mapplayery = 0;
                maplistx = 0;
                maplisty = 0;
                for (int i = 0; i < 4; i++) {
                    for (int o = 0; o < 4; o++) {
                        maplist[i][o] = 0;
                    }
                }
                for (int i = 0; i < 4; i++) {
                    for (int o = 0; o < 4; o++) {
                        maxemap[i][o] = 5+5*(round-1);
                    }
                }
                while (1) {
                    maplist[maplisty][maplistx] = 1;
                    rand3 = rand() % 3;
                    if (maplisty + 1 == 4 || maplistx + 1 == 4) {
                        maplist[maplisty][maplistx] = 2;
                        break;
                    }
                    if (maplisty > 0) {
                        if (rand3 == 0) {
                            maplisty++;
                        }
                        else if (rand3 == 1) {
                            maplistx++;
                        }
                        else {
                            maplisty--;
                        }
                    }
                    else {
                        if (rand3 == 0) {
                            maplisty++;
                        }
                        else if (rand3 == 1) {
                            maplistx++;
                        }
                    }
                }
                char nomalmap[2] = "@";
                char nomap[2] = "x";
                char bossmap[2] = "#";
                char twinc[2] = " ";
                while (1) {
                    if (end == true) {
                        break;
                    }
                    if (maplist[mapplayery][mapplayerx] == 2) {
                        break;
                    }
                    weaponcool -= 1;
                    if (weaponcool < 0) {
                        for (int y = 0; y < 100; y++) {
                            for (int x = 0; x < 100; x++) {
                                weapon.hitxy[y][x] = 0;

                            }
                        }
                    }
                    ScreenInit();//화면 생성
                    ScreenClear();//빈화면으로 초기화
                    sprintf(ascore, "남은 적%d", maxemap[mapplayery][mapplayerx]);
                    ScreenPrint(1, 33, ascore);
                    for (int i = 0; i < 4; i++) {
                        for (int o = 0; o < 4; o++) {
                            if (maplist[i][o] == 0)
                                ScreenPrint(30 + o, 33 + i, nomap);
                            else if (maplist[i][o] == 1) {
                                ScreenPrint(30 + o, 33 + i, nomalmap);

                            }
                            if (count % 10 == 0 && mapplayerx == o && mapplayery == i) {
                                ScreenPrint(30 + o, 33 + i, twinc);
                            }
                            else if (maplist[i][o] == 2) {
                                ScreenPrint(30 + o, 33 + i, bossmap);
                            }
                        }
                    }
                    if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
                        end = true;
                    }
                    if (((GetAsyncKeyState(VK_NUMPAD1) & 0x8000) != 0 || (GetAsyncKeyState(0x31) & 0x8000) != 0) && healp1 > 0) {
                        if (lifecool < 0) {
                            healp1 -= 1;
                            life += 10;
                            lifecool = 10;
                        }
                    }
                    if (((GetAsyncKeyState(VK_NUMPAD2) & 0x8000) != 0 || (GetAsyncKeyState(0x32) & 0x8000) != 0) && healp2 > 0) {
                        if (lifecool < 0) {
                            healp2 -= 1;
                            life += 20;
                            lifecool = 10;
                        }
                    }
                    if (((GetAsyncKeyState(VK_NUMPAD3) & 0x8000) != 0 || (GetAsyncKeyState(0x33) & 0x8000) != 0) && healp3 > 0) {
                        if (lifecool < 0) {
                            healp3 -= 1;
                            life += 40;
                            lifecool = 10;
                        }
                    }
                    lifecool -= 1;
                    if (((GetAsyncKeyState(VK_NUMPAD4) & 0x8000) != 0 || (GetAsyncKeyState(0x34) & 0x8000) != 0) && mclock > 0) {
                        double start, end;
                        start = (double)clock() / CLOCKS_PER_SEC;
                        if (clockcool < 0) {
                            mclock -= 1;
                            clockcool = 10;
                            while (1) {
                                end = (double)clock() / CLOCKS_PER_SEC;
                                if (end - start > 5) {
                                    break;
                                }
                            }
                        }
                    }
                    clockcool -= 1;
                    //무기 사용
                    if (((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)) {//스페이스바를 누르면
                        if (weaponcool < 0) {//쿨타임이 돌았으면
                            if (weapon.type == 0) {//1번무기면 1x6의 범위 

                                weapon.hitxy[playery + 1][playerx] = true;
                                weapon.hitxy[playery + 2][playerx] = true;
                                weapon.hitxy[playery + 3][playerx] = true;
                                weapon.hitxy[playery - 1][playerx] = true;
                                weapon.hitxy[playery - 2][playerx] = true;
                                weapon.hitxy[playery - 3][playerx] = true;
                            }
                            else if (weapon.type == 1) {

                                weapon.hitxy[playery + 1][playerx] = true;
                                weapon.hitxy[playery + 2][playerx] = true;
                                weapon.hitxy[playery + 3][playerx] = true;
                                weapon.hitxy[playery - 1][playerx] = true;
                                weapon.hitxy[playery - 2][playerx] = true;
                                weapon.hitxy[playery - 3][playerx] = true;
                                weapon.hitxy[playery][playerx + 2] = true;
                                weapon.hitxy[playery][playerx + 4] = true;
                                weapon.hitxy[playery][playerx + 6] = true;
                                weapon.hitxy[playery][playerx - 2] = true;
                                weapon.hitxy[playery][playerx - 4] = true;
                                weapon.hitxy[playery][playerx - 6] = true;
                            }
                            weaponcool = 10;//쿨타임을 다시 10으로
                        }
                    }
                    //적 생성
                    if (count % 40 == 1)//나타나는 속도 제한
                    {
                        if (crr==0) {
                            crea = maxemap[mapplayery][mapplayerx];
                            crr = 1;
                        }
                        for (int i = 1; i <= crea; i++)//나타나는 적 개수 제한
                        {

                            if (enermy[i].type == 0)
                            {
                                if (rand() % 4 == 0) {//1번 적을 생성
                                    enermy[i].type = 1;
                                    enermy[i].whereexist = 0;
                                    enermy[i].x = 2 + rand() % 67;
                                    enermy[i].y = 2;
                                }
                                else if (rand() % 4 == 0) {
                                    enermy[i].type = 1;
                                    enermy[i].whereexist = 1;
                                    enermy[i].y = 2 + rand() % 26;
                                    enermy[i].x = 2;
                                }
                                else if (rand() % 4 == 0) {
                                    enermy[i].type = 1;
                                    enermy[i].whereexist = 2;
                                    enermy[i].y = 2 + rand() % 26;
                                    enermy[i].x = 68;
                                }
                                else {
                                    enermy[i].type = 1;
                                    enermy[i].whereexist = 3;
                                    enermy[i].x = 2 + rand() % 67;
                                    enermy[i].y = 28;
                                }//1번적을생성
                                break;
                            }
                            else if (enermy[i].type != 100)
                            {
                                continue;
                            }
                        }
                    }
                    //맞은 적 삭제
                    for (int i = 0; i <= 30; i++) {
                        for (int y = 0; y < 100; y++) {
                            for (int x = 0; x < 100; x++) {
                                if (weapon.hitxy[y][x] == true) {
                                    if ((enermy[i].x == x || enermy[i].x == x + 1) && enermy[i].y == y) {
                                        if (enermy[i].type != 100) {
                                            maxemap[mapplayery][mapplayerx]--;
                                            gold += 1;
                                            brokenecon++;
                                        }
                                        enermy[i].type = 100;

                                    }
                                }
                            }
                        }
                    }
                    for (int i = 0; i <= 30; i++)
                    {
                        if (enermy[i].type != 0 && enermy[i].type != 100)
                        {
                            ScreenPrint(enermy[i].x, enermy[i].y, enermyp);
                            if ((enermy_bullet[i].x < 2) || (enermy_bullet[i].x > 68) || (enermy_bullet[i].y < 2) || (enermy_bullet[i].y > 28)) {
                                enermy_bullet[i].type = 0;
                            }
                            if (enermy[i].type == 1) {
                                if (enermy_bullet[i].type != 0)
                                    gob1(i);
                                else {
                                    if (rand() % 2 == 1 and enermy[i].whereexist == 0) {
                                        enermy_bullet[i].type = 1;
                                        enermy_bullet[i].x = enermy[i].x;
                                        enermy_bullet[i].y = 3;
                                    }
                                    else if (rand() % 2 == 1 and enermy[i].whereexist == 1) {
                                        enermy_bullet[i].type = 1;
                                        enermy_bullet[i].y = enermy[i].y;
                                        enermy_bullet[i].x = 3;
                                    }
                                    else if (rand() % 2 == 1 and enermy[i].whereexist == 2) {
                                        enermy_bullet[i].type = 1;
                                        enermy_bullet[i].y = enermy[i].y;
                                        enermy_bullet[i].x = 67;
                                    }
                                    else if (rand() % 2 == 1 and enermy[i].whereexist == 3) {
                                        enermy_bullet[i].type = 1;
                                        enermy_bullet[i].x = enermy[i].x;
                                        enermy_bullet[i].y = 27;
                                    }
                                }
                            }
                            else if (enermy[i].type == 0) {
                                continue;
                            }
                        }
                    }
                    //적 무기 출력
                    for (int i = 0; i <= 100; i++) {
                        if (enermy[i].type == 100 || enermy[i].type == 0) {
                            enermy_bullet[i].type = 0;
                            continue;
                        }
                        if (enermy_bullet[i].type != 100) {
                            if (enermy[i].whereexist == 0 || enermy[i].whereexist == 3)
                                ScreenPrint(enermy_bullet[i].x, enermy_bullet[i].y, b11);
                            else
                                ScreenPrint(enermy_bullet[i].x, enermy_bullet[i].y, b12);
                        }
                    }
                    if (weaponcool > 0) {
                        for (int y = 0; y < 100; y++) {
                            for (int x = 0; x < 100; x++) {
                                if (weapon.hitxy[y][x] == true) {
                                    if (weapon.type == 0) {
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                                        ScreenPrint(x, y, weaponskin1);
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4 + round);
                                    }
                                    else if (weapon.type == 1) {
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                                        ScreenPrint(x, y, weaponskin1);
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4 + round);
                                    }
                                }
                            }
                        }
                    }
                    sprintf_s(ascore, "♥: %d  $: %d  물약: %d  물통: %d  물탱크: %d  모래시계: %d", life, gold, healp1, healp2, healp3, mclock);
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                    ScreenPrint(0, 0, ascore);
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4 + round);
                    for (int i = 2; i < 29; i++) {
                        ScreenPrint(1, i, ypan);
                    }
                    for (int i = 2; i < 29; i++) {
                        ScreenPrint(69, i, ypan);
                    }
                    for (int i = 2; i < 69; i++) {
                        ScreenPrint(i, 1, xpan);
                    }
                    for (int i = 2; i < 69; i++) {
                        ScreenPrint(i, 29, xpan);
                    }
                    count++;
                    //플레이어 움직이는 부분
                    if (count % 2 == 1)//속도
                    {

                        if (((GetAsyncKeyState(VK_LEFT) & 0x8000) != 0))//왼  geyasynckeystate는 인수로 보낸 값을 누르지 않으면 0을 반환,누르면 0x8000이나 0x8001반환.
                        {
                            if (playerx > 2)playerx -= 2;//왼쪽으로(if문은 판밖으로 못나가게 제어)

                        }
                        else if (((GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0))//오
                        {
                            if (playerx < 67)playerx += 2;//오른쪽으로
                        }
                        if (((GetAsyncKeyState(VK_UP) & 0x8000) != 0))//위
                        {
                            if (playery > 2)playery--;//위로
                        }
                        else if (((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0))//아래
                        {
                            if (playery < 28)playery++;//아래로
                        }
                    }
                    else {
                        Sleep(20);
                    }
                    if (hit(playerx, playery))
                    {
                        life -= 1;
                        if (con2 == 0) {
                            con2 = 2;
                            SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                        }
                    }
                    else if (con2 > 0) {
                        con2--;
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                    }
                    if (life <= 0) {
                        break;
                    }
                    if (maxemap[mapplayery][mapplayerx] <= 0) {
                        if (opengate(maplist, &mapplayerx, &mapplayery, &playerx, &playery,&crr)) {
                            for (int i = 0; i < 50; i++) {
                                enermy[i].type = 0;
                            }
                            brokenecon = 0;
                        }
                    }
                    ScreenPrint(playerx, playery, player);
                    if (round == 1) {
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 5);
                    }
                    else if (round == 2) {
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 6);
                    }
                    else if (round == 3) {
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 7);
                    }
                    ScreenFlipping();
                }
                Sleep(20);
                if (life <= 0) {
                    break;
                }
                ///////////////////////////////////////////////////////보스//////////////////////////////////////////////////////////
                boss b1;
                b1.life = 20+5*(round-1);
                b1.x = 20;
                b1.y = 5;
                bool bosshit = false;
                char bossob[4][40] = {
                    "   --^^^     ^^^^^     ^^^--",
                    " ^        ^--|   |--^        ^",
                    "^-------     |   |     --------^",
                    "             vvvvv"
                };
                char bossmessage[30] = "영겁의 눈, 보스 체리";
                int randmove;
                while (1) {

                    if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
                        end = true;
                    }
                    if (((GetAsyncKeyState(VK_NUMPAD1) & 0x8000) != 0 || (GetAsyncKeyState(0x31) & 0x8000) != 0) && healp1 > 0) {
                        if (lifecool < 0) {
                            healp1 -= 1;
                            life += 10;
                            lifecool = 10;
                        }
                    }
                    if (((GetAsyncKeyState(VK_NUMPAD2) & 0x8000) != 0 || (GetAsyncKeyState(0x32) & 0x8000) != 0) && healp2 > 0) {
                        if (lifecool < 0) {
                            healp2 -= 1;
                            life += 20;
                            lifecool = 10;
                        }
                    }
                    if (((GetAsyncKeyState(VK_NUMPAD3) & 0x8000) != 0 || (GetAsyncKeyState(0x33) & 0x8000) != 0) && healp3 > 0) {
                        if (lifecool < 0) {
                            healp3 -= 1;
                            life += 40;
                            lifecool = 10;
                        }
                    }
                    lifecool -= 1;
                    if (((GetAsyncKeyState(VK_NUMPAD4) & 0x8000) != 0 || (GetAsyncKeyState(0x34) & 0x8000) != 0) && mclock > 0) {
                        double start, end;
                        start = (double)clock() / CLOCKS_PER_SEC;
                        if (clockcool < 0) {
                            mclock -= 1;
                            clockcool = 10;
                            while (1) {
                                end = (double)clock() / CLOCKS_PER_SEC;
                                if (end - start > 5) {
                                    break;
                                }
                            }
                        }
                    }
                    clockcool -= 1;

                    if (end == true) {
                        break;
                    }
                    if (b1.life <= 0) {
                        round += 1;
                        crr = 0;
                        gold += 20 + round * round * 5;
                        break;
                    }
                    randmove = rand() % 4;
                    if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
                        end = true;
                    }
                    ScreenInit();//화면 생성
                    ScreenClear();//빈화면으로 초기화
                    if (round == 1) {
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 5);
                    }
                    else if (round == 2) {
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 6);
                    }
                    else if (round == 3) {
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 7);
                    }
                    ScreenPrint(b1.x + 6, b1.y - 3, bossmessage);
                    if (count % 10 == 1) {
                        if (randmove == 0 && b1.x < 45) {
                            b1.x += 4;
                        }
                        else if (randmove == 1 && b1.x > 2) {
                            b1.x -= 4;
                        }
                        else if (randmove == 2 && b1.y < 22) {
                            b1.y += 2;
                        }
                        else if (randmove == 3 && b1.y > 2) {
                            b1.y -= 2;
                        }
                    }
                    for (int i = 0; i < 4; i++) {
                        ScreenPrint(b1.x, b1.y + i, bossob[i]);

                    }
                    if (count % (10-(round-1)*3) == 1)
                    {
                        for (int i = 0; i <= 100; i++)
                        {
                            if (bb[i].exist == FALSE)
                            {
                                bb[i].exist = TRUE;
                                bb[i].x = 2 + rand() % 66;
                                bb[i].y = 2;
                                break;
                            }
                            else if (bb[i].exist == TRUE)
                            {
                                continue;
                            }
                        }
                    }
                    if (count % 5 == 1)
                    {
                        for (int i = 0; i <= 100; i++)
                        {
                            if (bb[i].exist == TRUE)
                            {
                                if (bb[i].y >= 28)
                                {
                                    bb[i].exist = FALSE;
                                    continue;
                                }
                                bb[i].y+=round;
                            }
                            else if (bb[i].exist == FALSE)
                                continue;
                        }
                    }
                    for (int i = 0; i <= 100; i++)
                    {
                        if (bb[i].exist == TRUE)
                        {
                            ScreenPrint(bb[i].x, bb[i].y, bbs);
                        }
                        else if (bb[i].exist == FALSE)
                            continue;
                    }
                    weaponcool -= 1;
                    if (weaponcool < 0) {
                        for (int y = 0; y < 100; y++) {
                            for (int x = 0; x < 100; x++) {
                                weapon.hitxy[y][x] = 0;

                            }
                        }
                    }

                    //무기 사용
                    if (((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)) {//스페이스바를 누르면
                        if (weaponcool < 0) {//쿨타임이 돌았으면
                            if (weapon.type == 0) {//1번무기면 1x6의 범위 

                                weapon.hitxy[playery + 1][playerx] = true;
                                weapon.hitxy[playery + 2][playerx] = true;
                                weapon.hitxy[playery + 3][playerx] = true;
                                weapon.hitxy[playery - 1][playerx] = true;
                                weapon.hitxy[playery - 2][playerx] = true;
                                weapon.hitxy[playery - 3][playerx] = true;
                            }
                            else if (weapon.type == 1) {

                                weapon.hitxy[playery + 1][playerx] = true;
                                weapon.hitxy[playery + 2][playerx] = true;
                                weapon.hitxy[playery + 3][playerx] = true;
                                weapon.hitxy[playery - 1][playerx] = true;
                                weapon.hitxy[playery - 2][playerx] = true;
                                weapon.hitxy[playery - 3][playerx] = true;
                                weapon.hitxy[playery][playerx + 2] = true;
                                weapon.hitxy[playery][playerx + 4] = true;
                                weapon.hitxy[playery][playerx + 6] = true;
                                weapon.hitxy[playery][playerx - 2] = true;
                                weapon.hitxy[playery][playerx - 4] = true;
                                weapon.hitxy[playery][playerx - 6] = true;
                            }
                            weaponcool = 10;//쿨타임을 다시 10으로
                        }
                    }

                    if (weaponcool > 0) {
                        for (int y = 0; y < 100; y++) {
                            for (int x = 0; x < 100; x++) {
                                if (weapon.hitxy[y][x] == true) {
                                    if (weapon.type == 0) {
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                                        ScreenPrint(x, y, weaponskin1);
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4 + round);
                                    }
                                    else if (weapon.type == 1) {
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                                        ScreenPrint(x, y, weaponskin1);
                                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4 + round);
                                    }
                                }
                            }
                        }
                    }
                    sprintf_s(ascore, "life: %d", life);
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                    ScreenPrint(1, 0, ascore);
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4 + round);
                    for (int i = 2; i < 29; i++) {
                        ScreenPrint(1, i, ypan);
                    }
                    for (int i = 2; i < 29; i++) {
                        ScreenPrint(69, i, ypan);
                    }
                    for (int i = 2; i < 69; i++) {
                        ScreenPrint(i, 1, xpan);
                    }
                    for (int i = 2; i < 69; i++) {
                        ScreenPrint(i, 29, xpan);
                    }
                    count++;
                    //플레이어 움직이는 부분
                    if (count % 2 == 1)//속도
                    {

                        if (((GetAsyncKeyState(VK_LEFT) & 0x8000) != 0))//왼  geyasynckeystate는 인수로 보낸 값을 누르지 않으면 0을 반환,누르면 0x8000이나 0x8001반환.
                        {
                            if (playerx > 2)playerx -= 2;//왼쪽으로(if문은 판밖으로 못나가게 제어)

                        }
                        else if (((GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0))//오
                        {
                            if (playerx < 67)playerx += 2;//오른쪽으로
                        }
                        if (((GetAsyncKeyState(VK_UP) & 0x8000) != 0))//위
                        {
                            if (playery > 2)playery--;//위로
                        }
                        else if (((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0))//아래
                        {
                            if (playery < 28)playery++;//아래로
                        }
                    }
                    else {
                        Sleep(20);
                    }
                    /*if (hit(playerx, playery))
                    {
                        life -= 1;
                        if (con2 == 0) {
                            con2 = 2;
                            SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                        }
                    }
                    else if (con2 > 0) {
                        con2--;
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                    }*/
                    if (life <= 0) {
                        break;
                    }

                    if (weaponcool == 10) {
                        for (int i = 0; i <= 30; i++) {
                            for (int y = 0; y < 100; y++) {
                                for (int x = 0; x < 100; x++) {
                                    if (weapon.hitxy[y][x] == true) {
                                        if (x > b1.x + 12 && x<b1.x + 20 && y>b1.y && y < b1.y + 6) {
                                            bosshit = true;
                                        }
                                    }
                                }
                            }
                        }
                        if (bosshit == true) {
                            bosshit = false;
                            b1.life -= 1;
                        }
                    }
                    sprintf(ascore, "체리, 남은체력: %d", b1.life);
                    ScreenPrint(30, 0, ascore);
                    ScreenPrint(playerx, playery, player);
                    if (count % 3 == 0) {
                        if (playerx > b1.x + 13 && playerx<b1.x + 18 && playery>b1.y - 2 && playery < b1.y + 5) {
                            life -= 1;
                            SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                            ScreenPrint(playerx, playery, player);
                        }
                    }
                    for (int i = 0; i < 100; i++) {
                        if ((bb[i].x == playerx || bb[i].x == playerx + 1) && bb[i].y == playery && bb[i].exist == true) {
                            bb[i].exist = false;
                            life -= 1;
                            SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                            ScreenPrint(playerx, playery, player);
                        }
                    }
                    sprintf_s(ascore, "♥: %d  $: %d  물약: %d  물통: %d  물탱크: %d  모래시계: %d", life, gold, healp1, healp2, healp3, mclock);
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                    ScreenPrint(3, 30, ascore);
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4 + round);
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 5);
                    ScreenFlipping();
                }
                ///////////////////////////////////////////////////////////
            }
            if (end == false) {
                ScreenRelease();
            }
            SetConsoleActiveScreenBuffer(hstd);
            drawaru(7);
            gotoxy(20, 20);

            printf("끝.");
            end = false;
            gotoxy(0, 30);
            if (score > highscore) {
                highscore = score;
            }
            Sleep(1000);
            system("cls");
        }
        //-----------------------------------------------------------------------------------------------
        if (menu == 12) {
            while (1) {
                if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
                    break;
                }
                weaponcool -= 1;
                if (weaponcool < 0) {
                    for (int y = 0; y < 100; y++) {
                        for (int x = 0; x < 100; x++) {
                            weapon.hitxy[y][x] = 0;
                        }
                    }
                }
                ScreenInit();//화면 생성
                ScreenClear();//빈화면으로 초기화

                //무기 사용
                if (((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)) {//스페이스바를 누르면
                    if (weaponcool < 0) {//쿨타임이 돌았으면
                        if (weapon.type == 0) {//1번무기면 1x6의 범위 
                            weapon.hitxy[playery + 1][playerx] = true;
                            weapon.hitxy[playery + 2][playerx] = true;
                            weapon.hitxy[playery + 3][playerx] = true;
                            weapon.hitxy[playery - 1][playerx] = true;
                            weapon.hitxy[playery - 2][playerx] = true;
                            weapon.hitxy[playery - 3][playerx] = true;
                        }
                        weaponcool = 10;//쿨타임을 다시 10으로
                    }
                }
                //적 생성
                if (count % 40 == 1)//나타나는 속도 제한
                {
                    for (int i = 1; i <= 100; i++)//나타나는 적 개수 제한
                    {
                        if (enermy[i].type == 0)
                        {
                            if (rand() % 4 == 0) {//1번 적을 생성
                                enermy[i].type = 1;
                                enermy[i].whereexist = 0;
                                enermy[i].x = 2 + rand() % 67;
                                enermy[i].y = 2;
                            }
                            else if (rand() % 4 == 0) {
                                enermy[i].type = 1;
                                enermy[i].whereexist = 1;
                                enermy[i].y = 2 + rand() % 26;
                                enermy[i].x = 2;
                            }
                            else if (rand() % 4 == 0) {
                                enermy[i].type = 1;
                                enermy[i].whereexist = 2;
                                enermy[i].y = 2 + rand() % 26;
                                enermy[i].x = 68;
                            }
                            else {
                                enermy[i].type = 1;
                                enermy[i].whereexist = 3;
                                enermy[i].x = 2 + rand() % 67;
                                enermy[i].y = 28;
                            }//1번적을생성
                            break;
                        }
                        else if (enermy[i].type != 0)
                        {
                            continue;
                        }
                    }
                }
                for (int i = 0; i <= 30; i++) {
                    for (int y = 0; y < 100; y++) {
                        for (int x = 0; x < 100; x++) {
                            if (weapon.hitxy[y][x] == true) {
                                if ((enermy[i].x == x || enermy[i].x == x + 1) && enermy[i].y == y) {
                                    enermy[i].type = 0;
                                }
                            }
                        }
                    }
                }
                for (int i = 0; i <= 30; i++)
                {
                    if (enermy[i].type != 0)
                    {
                        ScreenPrint(enermy[i].x, enermy[i].y, enermyp);
                        if ((enermy_bullet[i].x < 2) || (enermy_bullet[i].x > 68) || (enermy_bullet[i].y < 2) || (enermy_bullet[i].y > 28)) {
                            enermy_bullet[i].type = 0;
                        }
                        if (enermy[i].type == 1) {
                            if (enermy_bullet[i].type != 0)
                                gob1(i);
                            else {
                                if (rand() % 2 == 1 and enermy[i].whereexist == 0) {
                                    enermy_bullet[i].type = 1;
                                    enermy_bullet[i].x = enermy[i].x;
                                    enermy_bullet[i].y = 3;
                                }
                                else if (rand() % 2 == 1 and enermy[i].whereexist == 1) {
                                    enermy_bullet[i].type = 1;
                                    enermy_bullet[i].y = enermy[i].y;
                                    enermy_bullet[i].x = 3;
                                }
                                else if (rand() % 2 == 1 and enermy[i].whereexist == 2) {
                                    enermy_bullet[i].type = 1;
                                    enermy_bullet[i].y = enermy[i].y;
                                    enermy_bullet[i].x = 67;
                                }
                                else if (rand() % 2 == 1 and enermy[i].whereexist == 3) {
                                    enermy_bullet[i].type = 1;
                                    enermy_bullet[i].x = enermy[i].x;
                                    enermy_bullet[i].y = 27;
                                }
                            }
                        }
                        else if (enermy[i].type == 0) {
                            continue;
                        }
                    }
                }
                //적 무기 출력
                for (int i = 0; i <= 100; i++) {
                    if (enermy[i].type == 0) {
                        enermy_bullet[i].type = 0;
                        continue;
                    }
                    if (enermy_bullet[i].type != 0) {
                        if (enermy[i].whereexist == 0 || enermy[i].whereexist == 3)
                            ScreenPrint(enermy_bullet[i].x, enermy_bullet[i].y, b11);
                        else
                            ScreenPrint(enermy_bullet[i].x, enermy_bullet[i].y, b12);
                    }
                }
                if (weaponcool > 0) {
                    for (int y = 0; y < 100; y++) {
                        for (int x = 0; x < 100; x++) {
                            if (weapon.hitxy[y][x] == true) {
                                if (weapon.type == 0) {
                                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 4);
                                    ScreenPrint(x, y, weaponskin1);
                                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 5);
                                }
                            }
                        }
                    }
                }
                if (count % 40 == 1) {
                    score++;
                }
                sprintf_s(ascore, "score: % d life: %d", score, life);
                ScreenPrint(1, 0, ascore);
                for (int i = 2; i < 29; i++) {
                    ScreenPrint(1, i, ypan);
                }
                for (int i = 2; i < 29; i++) {
                    ScreenPrint(69, i, ypan);
                }
                for (int i = 2; i < 69; i++) {
                    ScreenPrint(i, 1, xpan);
                }
                for (int i = 2; i < 69; i++) {
                    ScreenPrint(i, 29, xpan);
                }
                count++;
                //플레이어 움직이는 부분
                if (count % 2 == 1)//속도
                {

                    if (((GetAsyncKeyState(VK_LEFT) & 0x8000) != 0))//왼  geyasynckeystate는 인수로 보낸 값을 누르지 않으면 0을 반환,누르면 0x8000이나 0x8001반환.
                    {
                        if (playerx > 2)playerx--;//왼쪽으로(if문은 판밖으로 못나가게 제어)

                    }
                    else if (((GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0))//오
                    {
                        if (playerx < 67)playerx++;//오른쪽으로
                    }
                    if (((GetAsyncKeyState(VK_UP) & 0x8000) != 0))//위
                    {
                        if (playery > 2)playery--;//위로
                    }
                    else if (((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0))//아래
                    {
                        if (playery < 28)playery++;//아래로
                    }
                }
                else {
                    Sleep(20);
                }
                if (hit(playerx, playery))
                {
                    life -= 1;
                    if (con2 == 0) {
                        con2 = 2;
                        SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                    }
                }
                else if (con2 > 0) {
                    con2--;
                    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 3);
                }
                if (life <= 0) {
                    ScreenClear();
                    break;
                }

                ScreenPrint(playerx, playery, player);
                sprintf(ascore, "최고기록: %d", highscore);
                ScreenPrint(5, 30, ascore);
                SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], 5);
                ScreenFlipping();

            }
            ScreenRelease();
            SetConsoleActiveScreenBuffer(hstd);
            drawaru(7);
            gotoxy(20, 20);

            printf("끝. 점수는 %d", score);
            gotoxy(0, 30);
            if (score > highscore) {
                highscore = score;
            }
            Sleep(1000);
            system("cls");
        }
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////메인종료



bool opengate(int(*maplist)[4], int* mapplayerx, int* mapplayery, int* playerx, int* playery,int *crr) {
    char xgate1[5] = "!^^!";
    char xgate2[5] = "!vv!";
    char ygate11[10] = "-";
    char ygate12[10] = "<";
    char ygate13[10] = "<";
    char ygate14[10] = "-";
    char ygate21[10] = "-";
    char ygate22[10] = ">";
    char ygate23[10] = ">";
    char ygate24[10] = "-";
    if (maplist[*mapplayery - 1][*mapplayerx] == 1 || maplist[*mapplayery - 1][*mapplayerx] == 2) {
        ScreenPrint(34, 2, xgate1);
        if (*playerx == 34 && *playery == 2) {
            *playery = 26;
            *mapplayery -= 1;
            *crr = 0;
            return 1;
        }
    }
    if (maplist[*mapplayery + 1][*mapplayerx] == 1 || maplist[*mapplayery + 1][*mapplayerx] == 2) {
        ScreenPrint(34, 28, xgate2);
        if (*playerx == 34 && *playery == 28) {
            *playery = 4;
            *mapplayery += 1;
            *crr = 0;
            return 1;
        }
    }

    if (maplist[*mapplayery][*mapplayerx - 1] == 1 || maplist[*mapplayery][*mapplayerx - 1] == 2) {
        ScreenPrint(2, 12, ygate11);
        ScreenPrint(2, 13, ygate12);
        ScreenPrint(2, 14, ygate13);
        ScreenPrint(2, 15, ygate14);
        if (*playerx == 2 && *playery == 14) {
            *playerx = 66;
            *mapplayerx -= 1;
            *crr = 0;
            return 1;
        }
    }
    if (maplist[*mapplayery][*mapplayerx + 1] == 1 || maplist[*mapplayery][*mapplayerx + 1] == 2) {
        ScreenPrint(68, 12, ygate21);
        ScreenPrint(68, 13, ygate22);
        ScreenPrint(68, 14, ygate23);
        ScreenPrint(68, 15, ygate24);
        if (*playerx == 68 && *playery == 14) {
            *playerx = 4;
            *mapplayerx += 1;
            *crr = 0;
            return 1;
        }
    }
    return 0;
}
//캐릭터가 맞았을지 체크
bool hit(int playerx, int playery) {
    for (int i = 0; i < 100; i++) {
        if (enermy_bullet[i].type != 0) {
            if ((enermy_bullet[i].x == playerx || enermy_bullet[i].x == playerx + 1 || enermy_bullet[i].x == playerx + 2) && enermy_bullet[i].y == playery) {
                enermy_bullet[i].type = 0;
                return true;
            }
        }
    }
    return false;
}
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int menuscreen() {
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = 1;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);

    int whereiscurser = 30;

    drawaru(7);

    gotoxy(30, 30);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
    printf("게임시작");
    gotoxy(30, 32);
    printf("상점");
    gotoxy(30, 34);
    printf("종료(정상종료해주셔야 저장됩니다.)");
    gotoxy(28, whereiscurser);
    printf("-");
    while (1) {
        if (((GetAsyncKeyState(VK_SPACE) & 1) != 0) || ((GetAsyncKeyState(VK_RETURN) & 1) != 0)) {
            if (whereiscurser == 30) {
                gotoxy(28, 30);
                printf("             ");
                gotoxy(28, 32);
                printf("         ");
                gotoxy(28, 34);
                printf("                                       ");
                whereiscurser = 31;
                gotoxy(30, 31);
                printf("캐주얼 모드");
                gotoxy(30, 33);
                printf("무한모드");

                gotoxy(28, whereiscurser);
                printf("-");

                while (1) {
                    if (((GetAsyncKeyState(VK_ESCAPE) & 1) != 0)) {
                        system("cls");
                        gotoxy(28, 10);
                        printf("로딩 중입니다.");
                        return -2;
                    }
                    if (((GetAsyncKeyState(VK_SPACE) & 1) != 0) || ((GetAsyncKeyState(VK_RETURN) & 1) != 0)) {
                        if (whereiscurser == 31) {
                            return 1;
                        }
                        if (whereiscurser == 33) {
                            return 12;
                        }
                    }
                    if (((GetAsyncKeyState(VK_DOWN) & 1) != 0)) {
                        gotoxy(28, whereiscurser);
                        printf(" ");
                        if (whereiscurser < 33) {
                            whereiscurser += 2;
                            gotoxy(28, whereiscurser);
                        }
                        else {
                            gotoxy(28, 31);
                            whereiscurser = 31;
                        }
                        printf("-");
                    }
                    if (((GetAsyncKeyState(VK_UP) & 1) != 0)) {
                        gotoxy(28, whereiscurser);
                        printf(" ");
                        if (whereiscurser > 31) {
                            whereiscurser -= 2;
                            gotoxy(28, whereiscurser);
                        }
                        else {
                            gotoxy(28, 33);
                            whereiscurser = 33;
                        }
                        printf("-");
                    }
                }
            }
            else if (whereiscurser == 32) {
                return 2;
            }
            else if (whereiscurser == 34) {
                return 3;
            }
        }
        if (((GetAsyncKeyState(VK_DOWN) & 1) != 0)) {
            gotoxy(28, whereiscurser);
            printf(" ");
            if (whereiscurser < 34) {
                whereiscurser += 2;
                gotoxy(28, whereiscurser);
            }
            else {
                gotoxy(28, 30);
                whereiscurser = 30;
            }
            printf("-");
        }
        if (((GetAsyncKeyState(VK_UP) & 1) != 0)) {
            gotoxy(28, whereiscurser);
            printf(" ");
            if (whereiscurser > 30) {
                whereiscurser -= 2;
                gotoxy(28, whereiscurser);
            }
            else {
                gotoxy(28, 34);
                whereiscurser = 34;
            }
            printf("-");
        }
    }
}