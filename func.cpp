#include "data.h"

//#define IDB_PNG5 105
//
//HRSRC hResource = ::FindResource(NULL, MAKEINTRESOURCE(IDB_PNG5), L"PNG");
//HGLOBAL hg = LoadResource(NULL, hResource);
//DWORD dwSize = SizeofResource(NULL, hResource);
//LPVOID lpAddress = LockResource(hg);
//Mat logo(imdecode(Mat(3, dwSize, CV_8UC3, lpAddress), IMREAD_UNCHANGED));

Mat logo = imread("img\\snakelogo.png", IMREAD_COLOR); //C:\\JKB\\CPP\\WORKSPACE\\CPPCODE\\SNAKE_semiteamproject\\ //
Mat title = imread("img\\snaketitle.png", IMREAD_COLOR);
Mat DDATG = imread("img\\ddatg.png", IMREAD_COLOR);
Mat HEAD = imread("img\\head.png", IMREAD_COLOR);
Mat DEAD = imread("img\\dead.png", IMREAD_COLOR);
Mat HEADNOW;
Mat GG = imread("img\\gameover.png", IMREAD_COLOR);
Mat mapgraphic(1000, 1000, CV_8UC3, Scalar(255, 255, 255));
int gamemode = 0;//0 설정 1게임 2종료및점수
int difficult = 5;
int timecounter; //시스템 시간 흐름
int field[MAPSIZE][MAPSIZE] = {0}; //맵
int snakelong[MAPSIZE * MAPSIZE][6] = { {1, 9, 9, 0, 0, 255}, {1, 8, 9, 51, 0, 255}, {1, 7, 9, 102, 0, 255}, {1, 6, 9, 153, 0, 255}}; //몸유무, x, y, b,g,r
int speed = 11; // 뱀이동속도1~5단계로 15 13 11 9 7
int score;// 쥐 잡아먹은 마리수
int direction = 3; //좌1 하2 우3 상4
int block = 1000 / MAPSIZE;
int rocknum;
int key;

void getkey() //waitKey대용
{
	if (gamemode == 1)
	{
			key = waitKeyEx(0);
			switch (key)
			{
			case 0x250000: //좌
				difficult--;
				if (difficult < 1)
					difficult = 1;
				break;
			case 0x270000: //우
				difficult++;
				if (difficult > 10)
					difficult = 10;
				break;
			case 13:
				gamemode = 2;
				return;
			case 27: 
				return;
			}
	}
	else if (gamemode == 2)
	{
		speed = 16 - 1 * difficult;
		int nextdirection = direction; //왔던방향 방지
		timecounter = 0; //speed 시간 안에 마지막으로 입력된 방향키 방향으로 보내기위한 카운터
		while (1)
		{
			key = waitKeyEx(1); //1ms대기

			switch (key)
			{
			case 0x250000: //좌
				if (direction != 3)
					nextdirection = 1;
				break;
			case 0x280000: //하
				if (direction != 4)
					nextdirection = 2;
				break;
			case 0x270000: //우
				if (direction != 1)
					nextdirection = 3;
				break;
			case 0x260000: //상
				if (direction != 2)
					nextdirection = 4;
				break;
			case 27: //좌
				return;
			}
			timecounter++;
			if (timecounter >= speed)
				break;
		}
		direction = nextdirection;
	}
}

void headgraphic(int ii, int jj) //뱀머리 회전 및 좌표 합성 함수
{
	resize(HEADNOW, HEADNOW, Size(block-1, block-1));
	Mat M = getRotationMatrix2D(Point(HEADNOW.cols / 2, HEADNOW.rows / 2), 90 * direction,1); //머리회전

	Mat rotatedHEAD;

	warpAffine(HEADNOW, rotatedHEAD, M, Size(HEADNOW.cols, HEADNOW.rows)); //머리회전

	Mat img_gray;
	cvtColor(rotatedHEAD, img_gray, COLOR_BGR2GRAY); //머리 합성

	Mat img_mask;
	threshold(img_gray, img_mask, 240, 255, THRESH_BINARY);

	Mat img_mask_inv;
	bitwise_not(img_mask, img_mask_inv);

	int height = rotatedHEAD.rows;
	int width = rotatedHEAD.cols;

	Mat img_roi(mapgraphic, Rect(block * ii, block * jj, width, height));

	Mat img1, img2;
	bitwise_and(rotatedHEAD, rotatedHEAD, img1, img_mask_inv);
	bitwise_and(img_roi, img_roi, img2, img_mask);

	Mat dst;
	add(img1, img2, dst);
	dst.copyTo(mapgraphic(Rect(block * ii, block * jj, width, height))); //머리 합성
}

void ddatg_graphic(int ii, int jj) //땃쥐 그래픽
{
	resize(DDATG, DDATG, Size(block, block));
	Mat img_gray;
	cvtColor(DDATG, img_gray, COLOR_BGR2GRAY);

	Mat img_mask;
	threshold(img_gray, img_mask, 240, 255, THRESH_BINARY);

	Mat img_mask_inv;
	bitwise_not(img_mask, img_mask_inv);

	int height = DDATG.rows;
	int width = DDATG.cols;

	Mat img_roi(mapgraphic, Rect(block * ii, block * jj, width, height));

	Mat img1, img2;
	bitwise_and(DDATG, DDATG, img1, img_mask_inv);
	bitwise_and(img_roi, img_roi, img2, img_mask);

	Mat dst;
	add(img1, img2, dst);
	dst.copyTo(mapgraphic(Rect(block * ii, block * jj, width, height)));
}

void graphic() //통합 그래픽처리
{
	for (int ii = 0; ii < MAPSIZE; ii++)
	{
		for (int jj = 0; jj < MAPSIZE; jj++)
		{
			if ((ii + jj) % 2 == 0)
			{
				mapgraphic(Rect(block * ii, block * jj, block, block)) = cv::Scalar(81, 215, 169); //연녹
			}
			else
				mapgraphic(Rect(block * ii, block * jj, block, block)) = cv::Scalar(74, 209, 164); //진녹
			for (int kk = 0; snakelong[kk][0] == 1; kk++)
			{
				if (snakelong[kk][1] == ii && snakelong[kk][2] == jj && kk == 0)
				{
					headgraphic(ii, jj);
					//cv::circle(mapgraphic, Point(block * ii + block / 2, block * jj + block / 2), block / 2, Scalar(0, 0, 255), -1);
				}
				else if (snakelong[kk][1] == ii && snakelong[kk][2] == jj)
					cv::circle(mapgraphic, Point(block * ii + block / 2, block * jj + block / 2), block / 2, Scalar(snakelong[kk][3], snakelong[kk][4], snakelong[kk][5]), -1); //각 몸통당 색상 반영
			}
			if (field[ii][jj] == 1)
				ddatg_graphic(ii, jj);
			//cv::circle(mapgraphic, Point(block * ii + block / 2, block * jj + block / 2), block / 2, Scalar(39, 127, 255), -1);
			if (field[ii][jj] == 3)
				mapgraphic(Rect(block * ii, block * jj, block, block)) = cv::Scalar(150, 150, 150); //회색
		}
	}
	char ch[10];
	char imshowname[20];
	strcpy(imshowname, "score : ");
	strcat(imshowname, _itoa(score, ch, 10));
	Point location = Point(0, 10);
	int font = FONT_HERSHEY_COMPLEX;
	double fontScale = 0.5;
	int thickness = 1;
	putText(mapgraphic, imshowname, location, font, fontScale, Scalar(0, 0, 0), thickness);
	imshow("snake", mapgraphic);
}

void logographic() //로고 합성
{
	resize(logo, logo, Size(600, 120));
	resize(title, title, Size(600, 130));

	/////////////////////////////////////////////////////////////////로고

	Mat img_gray;
	cvtColor(logo, img_gray, COLOR_BGR2GRAY);

	Mat img_mask;
	threshold(img_gray, img_mask, 240, 255, THRESH_BINARY);

	Mat img_mask_inv;
	bitwise_not(img_mask, img_mask_inv);

	int height = logo.rows;
	int width = logo.cols;

	Mat img_roi(mapgraphic, Rect(200, 100, width, height));

	Mat img1, img2;
	bitwise_and(logo, logo, img1, img_mask_inv);
	bitwise_and(img_roi, img_roi, img2, img_mask);

	Mat dst;
	add(img1, img2, dst);
	dst.copyTo(mapgraphic(Rect(200, 100, width, height)));

	//////////////////////////////////////////////////////////////////타이틀

	img_gray;
	cvtColor(title, img_gray, COLOR_BGR2GRAY);

	img_mask;
	threshold(img_gray, img_mask, 240, 255, THRESH_BINARY);

	img_mask_inv;
	bitwise_not(img_mask, img_mask_inv);

	height = title.rows;
	width = title.cols;

	img_roi = mapgraphic(Rect(200, 50, width, height));

	img1, img2;
	bitwise_and(title, title, img1, img_mask_inv);
	bitwise_and(img_roi, img_roi, img2, img_mask);

	dst;
	add(img1, img2, dst);
	dst.copyTo(mapgraphic(Rect(200, 50, width, height)));

	//////////////////////////////////////////////////////////////////타이틀
	Point location = Point(345, 250);
	int font = FONT_HERSHEY_COMPLEX;
	double fontScale = 1.2;
	int thickness = 2;
	putText(mapgraphic, "Select Difficult", location, font, fontScale, Scalar(0, 0, 0), thickness);
	char difficultch[10];
	char ch[10];
	strcpy(ch, "<< ");
	strcat(ch, _itoa(difficult, difficultch, 10)); //난이도 설정 텍스트
	strcat(ch, " >>");
	location = Point(410, 300);
	putText(mapgraphic, ch, location, font, fontScale, Scalar(0, 0, 255), thickness);
	fontScale = 0.8;
	location = Point(370, 350);
	putText(mapgraphic, "Press", location, font, fontScale, Scalar(0, 0, 0), thickness);
	location = Point(455, 350);
	putText(mapgraphic, "ENTER", location, font, fontScale, Scalar(0, 255, 255), thickness);
	location = Point(550, 350);
	putText(mapgraphic, "to START", location, font, fontScale, Scalar(0, 0, 0), thickness);
	imshow("snake", mapgraphic);
}

void mice() //땃쥐 랜덤 생성
{
	srand(time(NULL));
	int micenum = 0;
	for (int ii = 0; ii < MAPSIZE; ii++) //전체 맵에 이미 땃쥐가 잇는지 확인
	{
		for (int jj = 0; jj < MAPSIZE; jj++)
		{
			if (field[ii][jj] == 1)
				micenum++;
			if (micenum == 3)
				return;
		}
	}
	while (micenum<3)
	{
		int spot = rand() % (MAPSIZE * MAPSIZE);
		if (field[spot / 20][spot % 20] == 0) //랜덤좌표가 비어있다면(2뱀 3바위) 땃쥐 생성
		{
			field[spot / 20][spot % 20] = 1;
			break;
		}
	}
}

void ROCK() //장애물 생성
{
	rocknum = 0;
	for (int ii = 0; ii < MAPSIZE; ii++) //맵에 존재하는 바위 갯수 체크
	{
		for (int jj = 0; jj < MAPSIZE; jj++)
		{
			if (field[ii][jj] == 3)
				rocknum++;
		}
	}
	while (score != 0 && score / 5 != rocknum) //점수가 0이 아닐때 땃쥐를 5마리 먹은 횟수와 현재바위수 체크 , 바위가 더 필요하면 반복
	{
		int safety = 3;
		int thereisnearhead = 0; //1.5 패치내용 - 뱀머리 바로앞에 바위생성 방지
		int spot = rand() % (MAPSIZE * MAPSIZE);
		for (int ii = (spot / 20-safety); ii <= (spot / 20 + safety); ii++) //랜덤좌표 5X5안에 뱀 머리가 존재하는지 확인
		{
			for (int jj = (spot / 20 - safety); jj <= (spot % 20 + safety); jj++)
			{
				if (snakelong[0][1] == ii && snakelong[0][2] == jj) //근처에 뱀머리잇으면 변수 on
				{
					thereisnearhead = 1;
					break;
				}
				if (thereisnearhead == 1)
					break;
			}
		}
		if (thereisnearhead == 1) //근처 뱀머리잇으면 억까방지 다시 랜덤돌리기
			continue;
		if (field[spot / 20][spot % 20] == 0) //랜덤좌표가 비어있다면 돌생성 아니면 다시반복
		{
			if (spot / 20 != 0 && spot / 20 != MAPSIZE - 1 && spot % 20 != 0 && spot % 20 != MAPSIZE - 1)
			{
				field[spot / 20][spot % 20] = 3; //돌은 3
				break;
			}
		}
	}
}

void fieldcheck() //필드 확인
{
	for (int ii = 0; ii < MAPSIZE; ii++) //맵전체 확인
	{
		for (int jj = 0; jj < MAPSIZE; jj++)
		{
			if (field[ii][jj] != 1 && field[ii][jj] != 3) //땃쥐나 바위가없다면 0으로 초기화
				field[ii][jj] = 0;
			for (int kk = 0; snakelong[kk][0] == 1; kk++) //뱀 몸통있나요?
			{
				if (snakelong[kk][1] == ii && snakelong[kk][2] == jj) //있네요 2로 설정
					field[ii][jj] = 2;
			}
		}
	}
}

void snakebody() //뱀 길이 및 좌표 gamingcolor
{
	for (int ii = 0; ii < MAPSIZE; ii++)
	{
		for (int jj = 0; jj < MAPSIZE; jj++)
		{
			if (field[ii][jj] == 1 && snakelong[0][1] == ii && snakelong[0][2] == jj) //DDATG eating
			{
				field[ii][jj] = 0; //DDATG die
				for (int kk = MAPSIZE* MAPSIZE-1; snakelong[kk][0] == -1; kk--) //뱀 꼬리부터 스캔
				{
					if (snakelong[kk-1][0] == 1) //여기가 꼬리의 끝이오
					{
						score++; //점수득
						snakelong[kk][0] = 1; //꼬리 생성
						snakelong[kk][1] = snakelong[kk-1][1]; //x좌표
						snakelong[kk][2] = snakelong[kk-1][2]; //y좌표

						switch (kk % 30 / 5) //BGR GAMING DDATG
						{
						case 0:
							snakelong[kk][3] = snakelong[kk - 1][3] + 51;	//B
							snakelong[kk][4] = snakelong[kk - 1][4];		//G
							snakelong[kk][5] = snakelong[kk - 1][5];		//R
							break;
						case 1:
							snakelong[kk][3] = snakelong[kk - 1][3];
							snakelong[kk][4] = snakelong[kk - 1][4];
							snakelong[kk][5] = snakelong[kk - 1][5] - 51;
							break;
						case 2:
							snakelong[kk][3] = snakelong[kk - 1][3];
							snakelong[kk][4] = snakelong[kk - 1][4] + 51;
							snakelong[kk][5] = snakelong[kk - 1][5];
							break;
						case 3:
							snakelong[kk][3] = snakelong[kk - 1][3] - 51;
							snakelong[kk][4] = snakelong[kk - 1][4];
							snakelong[kk][5] = snakelong[kk - 1][5];
							break;
						case 4:
							snakelong[kk][3] = snakelong[kk - 1][3];
							snakelong[kk][4] = snakelong[kk - 1][4];
							snakelong[kk][5] = snakelong[kk - 1][5] + 51;
							break;
						case 5:
							snakelong[kk][3] = snakelong[kk - 1][3];
							snakelong[kk][4] = snakelong[kk - 1][4] - 51;
							snakelong[kk][5] = snakelong[kk - 1][5];
							break;
						}

						break;
					}
				}
			}
		}
	}
	for (int kk = 0; kk < MAPSIZE * MAPSIZE - 1; kk++) //뱀이있는 좌표는 2    //사실 fieldcheck함수 만들어서 필요없긴한데 버그 없으니 건들지 마셈
	{
		if (snakelong[kk][0] == 1)
			field[snakelong[kk][1]][snakelong[kk][2]] = 2;
	}
}

void snakemove() //뱀이동
{
	int x, y;
	x = snakelong[0][1]; //직전좌표 백업
	y = snakelong[0][2];
	if (direction == 1)//상
	{
		for (int kk = MAPSIZE * MAPSIZE - 1; kk >= 1; kk--)
		{
			if (snakelong[kk][0] == 1)
			{
				snakelong[kk][1] = snakelong[kk - 1][1];
				snakelong[kk][2] = snakelong[kk - 1][2];
			}
		}
		snakelong[0][1] -= 1;
	}
	if (direction == 2)//우
	{
		for (int kk = MAPSIZE * MAPSIZE - 1; kk >= 1; kk--)
		{
			if (snakelong[kk][0] == 1)
			{
				snakelong[kk][1] = snakelong[kk - 1][1];
				snakelong[kk][2] = snakelong[kk - 1][2];
			}
		}
		snakelong[0][2] += 1;
	}
	if (direction == 3)//하
	{
		for (int kk = MAPSIZE * MAPSIZE - 1; kk >= 1; kk--)
		{
			if (snakelong[kk][0] == 1)
			{
				snakelong[kk][1] = snakelong[kk - 1][1];
				snakelong[kk][2] = snakelong[kk - 1][2];
			}
		}
		snakelong[0][1] += 1;
	}
	if (direction == 4)//좌
	{
		for (int kk = MAPSIZE * MAPSIZE - 1; kk >= 1; kk--)
		{
			if (snakelong[kk][0] == 1)
			{
				snakelong[kk][1] = snakelong[kk - 1][1];
				snakelong[kk][2] = snakelong[kk - 1][2];
			}
		}
		snakelong[0][2] -= 1;
	}
	for (int kk = 1; kk < MAPSIZE * MAPSIZE; kk++) //벽쿵 바위쿵 몸쿵시
	{
		if ((snakelong[0][1] == snakelong[kk][1] && snakelong[0][2] == snakelong[kk][2]) || snakelong[0][1] < 0 || snakelong[0][1] == MAPSIZE || snakelong[0][2] < 0 || snakelong[0][2] == MAPSIZE || field[snakelong[0][1]][snakelong[0][2]] == 3)
		{
			snakelong[0][1] = x;
			snakelong[0][2] = y;

			HEADNOW = DEAD; //죽은머리로 교체
			headgraphic(x, y); //머리출력
			imshow("snake", mapgraphic); //엔딩
			key = 'D';
			return;
		}
	}
}

void gameover()
{
	Mat img_gray;
	cvtColor(GG, img_gray, COLOR_BGR2GRAY); //엔딩 땃쥐 합성

	Mat img_mask;
	threshold(img_gray, img_mask, 250, 255, THRESH_BINARY);

	Mat img_mask_inv;
	bitwise_not(img_mask, img_mask_inv);

	Mat img1, img2;
	bitwise_and(GG, GG, img1, img_mask_inv);
	bitwise_and(mapgraphic, mapgraphic, img2, img_mask);

	Mat dst1;
	add(img1, img2, dst1);
	
	Mat dst2;
	addWeighted(dst1, 0.4, mapgraphic, 0.6, 0, dst2);

	Point location = Point(95, 200); //엔딩 문구 출력
	int font = FONT_HERSHEY_SIMPLEX;
	double fontScale = 5;
	int thickness = 10;
	putText(dst2, "GAMEOVER", location, font, fontScale, Scalar(0, 0, 0), thickness);

	char DDAT[10];
	char ch[20];
	strcpy(ch, "You ate ");
	if (score + 4 + rocknum == MAPSIZE * MAPSIZE)
		strcat(ch, "ALL ");
	else 
		strcat(ch, _itoa(score, DDAT, 10));
	strcat(ch, " DDATG");
	location = Point(600, 400);
	fontScale = 1.2;
	thickness = 2;
	putText(dst2, ch, location, font, fontScale, Scalar(0, 0, 0), thickness);
	
	location = Point(600, 500);
	fontScale = 1.2;
	thickness = 2;
	putText(dst2, "Press ESC or R", location, font, fontScale, Scalar(0, 0, 0), thickness);

	imshow("snake", dst2);
}
