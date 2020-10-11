#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstdlib>

#define WIDTH  176
#define HEIGHT 176
#define BUFFER 176

using namespace std;

class Stack {
	int x, y;
	int direction;
	int count;
public:
	Stack() {
		x = 0;
		y = 0;
		direction = 0;
		count = 0;
	}
	void init(int a, int b, int c, int d) {
		x = a;
		y = b;
		direction = c;
		count = d;
	}
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
	int getDirection() {
		return direction;
	}
	int getCount() {
		return count;
	}
};
class Queue {
	int x, y;
public:
	Queue() {
		x = 0;
		y = 0;
	}
	void init(int x, int y) {
		this->x = x;
		this->y = y;
	}
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
};
int w;
int h;
char ans[BUFFER];

int dfs(int[][WIDTH], int, int);
int bfs(int[][WIDTH]);
int move(int[][WIDTH], int, int&, int&, int&);
int possibleTile(int[][WIDTH], int, int);
void convert(char[][WIDTH], int[][WIDTH]);

int main()
{
	while (1) {
		char tmp[HEIGHT][WIDTH] = { 0 };
		int tile[HEIGHT][WIDTH] = { 0 };
		clock_t start, end;

		printf("                                 ┌────────────────────────┐\n");
		printf("                                 │                                                │\n");
		printf("                                 │       http://www.hacker.org/coil Solution      │\n");
		printf("                                 │                                                │\n");
		printf("                                 │          Made by YunGoon in 2016-05-06         │\n");
		printf("                                 │                                                │\n");
		printf("                                 └────────────────────────┘\n\n");
		printf(">> Please enter flashvars element of html embed tag\n>> ");

		//입력
		scanf("x=%d&y=%d&board=", &w, &h);
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				scanf("%c", &tmp[i][j]);

		start = clock();

		int ansX, ansY, exit = 1;
		for (int j = 0; exit && j < w; j++) {
			for (int i = 0; exit && i < h; i++) {
				if (tmp[i][j] == 'X') continue; //시작 지점이 블럭일 경우 다른 지점 선택
				convert(tmp, tile); 			//tile에 원본인 tmp를 변환하여 저장
				if (dfs(tile, j, i)) {
					ansX = j;
					ansY = i;
					exit = 0;
				}
				if (exit) printf("(%2d, %2d) passed\n", j, i);
			}
		}

		end = clock();

		printf("\a\n>> ANSWER (%lf seconds)\n", (end - start) / 1000.0);
		printf("http://www.hacker.org/coil/index.php?x=%d&y=%d&qpath=%s\n", ansX, ansY, ans);

		system("pause");
		system("cls");

		char c = getchar();
	}

	return 0;
}
int dfs(int t[][WIDTH], int a, int b)
{
	//(a,b)가 현재 위치

	int k = 0; //스택에 남아있는 자료 개수
	int tile_count = 2;
	int dy[] = { 0,1,0,-1 };
	int dx[] = { 1,0,-1,0 };
	Stack s[WIDTH * HEIGHT];

	s[k++].init(a, b, -1, tile_count);

	while (k) {
		k--;
		int x = s[k].getX();			  //현재의 가로축 좌표
		int y = s[k].getY();			  //현재의 세로축 좌표
		int drc = s[k].getDirection();	  //1오른쪽 2아래쪽 3왼쪽 4위쪽
		int tct = s[k].getCount();		  //타일 넘버

		t[y][x] = tct; //지나간 흔적

		int m = move(t, drc, x, y, tct); //정해진 방향으로 두갈래 길이 나올 때까지

		//현재 위치에선 이동할 타일이 m(m=0은 0, m=5는 2)개 있다
		int check = 0;
		int bfs_val = bfs(t);

		if (bfs_val == 2) {
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					if (t[i][j] == 1) printf("□ ");
					else if (t[i][j] == 2) printf("★ ");
					else printf("%2d ", t[i][j] - 2);
				}
				printf("\n");
			}
			printf("\n");

			return 1;
		}
		if (bfs_val && m) { //타일이 이어져 있고 갈 타일이 있다
			for (int i = 0; i < 4; i++) {
				int nx = x + dx[i];
				int ny = y + dy[i];

				if ((nx >= 0 && nx < w) && (ny >= 0 && ny < h) && !t[ny][nx]) {
					if (i == 0) s[k++].init(nx, ny, 1, tct + 1);		 //오른쪽
					else if (i == 1) s[k++].init(nx, ny, 2, tct + 1);	 //아래쪽
					else if (i == 2) s[k++].init(nx, ny, 3, tct + 1);	 //왼쪽
					else s[k++].init(nx, ny, 4, tct + 1);				 //위쪽
				}
			}
		}
		else {
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					if (k == 0) return 0;
					if (t[i][j] >= s[k - 1].getCount()) t[i][j] = 0;
				}
			}
		}
	}
}
int bfs(int t[][WIDTH])
{
	int catI = -1, catJ = -1, exit = 0;

	for (int i = 0; !exit && i < h; i++) {
		for (int j = 0; !exit && j < w; j++) {
			if (t[i][j] == 0) {
				catI = i;
				catJ = j;
				exit = 1;
			}
		}
	}

	if (catI + catJ < 0) return 2; //정답!
	//(catJ,catI)가 현재 위치

	int hd = 0, rr = 0;
	int dy[] = { 0,1,0,-1 };
	int dx[] = { 1,0,-1,0 };
	int c[HEIGHT][WIDTH];
	Queue q[WIDTH * HEIGHT];

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			c[i][j] = t[i][j];

	q[rr++].init(catJ, catI);
	c[catI][catJ] = 1; //지나간 흔적

	while (hd != rr) {
		int x = q[hd].getX();	 //현재의 가로축 좌표
		int y = q[hd].getY();	 //현재의 세로축 좌표
		hd++;

		for (int i = 0; i < 4; i++) {
			int nx = x + dx[i];
			int ny = y + dy[i];

			if ((nx >= 0 && nx < w) && (ny >= 0 && ny < h)) {
				if (!c[ny][nx]) {
					q[rr++].init(nx, ny);
					c[ny][nx] = 1; //지나간 흔적
				}
			}
		}
	}

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (c[i][j] == 0) return 0; //불가능
		}
	}

	return 1; //가능
}
int move(int t[][WIDTH], int d, int &x, int &y, int &tct)
{
	//이동할 곳이 한 곳밖에 없는 경우 계속 이동
	//두 가지의 길이 있으면 루프 탈출

	int count = 1;
	while (d >= 1 && d <= 4) {
		if (d == 1) {
			while (x < w - 1 && !t[y][x + 1]) t[y][++x] = tct;	//오른쪽
			if (count) ans[tct - 3] = 'R', ans[tct - 2] = '\0', count--;
		}
		else if (d == 3) {
			while (x > 0 && !t[y][x - 1]) t[y][--x] = tct;		//왼쪽
			if (count) ans[tct - 3] = 'L', ans[tct - 2] = '\0', count--;
		}
		else if (d == 4) {
			while (y > 0 && !t[y - 1][x]) t[--y][x] = tct;		//위쪽
			if (count) ans[tct - 3] = 'U', ans[tct - 2] = '\0', count--;
		}
		else if (d == 2) {
			while (y < h - 1 && !t[y + 1][x]) t[++y][x] = tct;	//아래쪽
			if (count) ans[tct - 3] = 'D', ans[tct - 2] = '\0', count--;
		}

		d = possibleTile(t, x, y);
	}

	return d;
}
void convert(char tmp[][WIDTH], int tile[][WIDTH])
{
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (tmp[i][j] == '.') tile[i][j] = 0;	//'.'은 0으로
			else tile[i][j] = 1;					//'X'는 1로
		}
	}
}
int possibleTile(int t[][WIDTH], int x, int y)
{
	int dy[] = { 0,1,0,-1 };
	int dx[] = { 1,0,-1,0 };
	int cnt = 0;
	int drc;

	for (int i = 0; i < 4; i++) {
		int nx = x + dx[i];
		int ny = y + dy[i];

		if ((nx >= 0 && nx < w) && (ny >= 0 && ny < h)) { //범위 검사
			if (!t[ny][nx]) {
				cnt++; //0이면 카운팅
				drc = i + 1;
			}
		}
	}

	if (cnt == 1) return drc;
	else if (cnt == 0) return 0;
	else if (cnt == 2) return 5;
}