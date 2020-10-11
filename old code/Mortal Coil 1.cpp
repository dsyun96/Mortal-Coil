#include <iostream>
#include <cstdio>
#include <cstdlib>

#define WIDTH  200
#define HEIGHT 200

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
	void setX(int a) {
		x = a;
	}
	int getX() {
		return x;
	}
	void setY(int b) {
		y = b;
	}
	int getY() {
		return y;
	}
	void setDirection(int c) {
		direction = c;
	}
	int getDirection() {
		return direction;
	}
	void setCount(int d) {
		count = d;
	}
	int getCount() {
		return count;
	}
};
int w;
int h;

int dfs(int[][WIDTH], int, int);
int explore(int[][WIDTH]);
int main()
{
	char tmp[HEIGHT][WIDTH];
	int tile[HEIGHT][WIDTH];

	scanf("x=%d&y=%d&board=", &w, &h);

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			scanf("%c", &tmp[i][j]);
		}
	}

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (tmp[i][j] == '.') tile[i][j] = 0;
			else tile[i][j] = 1;
		}
	}

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (tile[i][j]) continue;
			if (dfs(tile, j, i)) break;
			printf("%d, %d\n", i, j);
		}
	}
	
	printf("\n>> ANSWER\n");
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (tile[i][j] == 1) printf(" □ ");
			else if (tile[i][j] == 2) printf(" ★ ");
			else printf("%3d ", tile[i][j] - 2);
		}
		printf("\n");
	}
	system("pause");

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

	s[k++].init(a, b, 0, tile_count);

	while (k) {
		k--;
		int x = s[k].getX();
		int y = s[k].getY();
		int drc = s[k].getDirection();
		int tct = s[k].getCount();

		t[y][x] = tct; //지나간 흔적

		if (drc) {
			if (drc == 1) {
				while (x < w - 1 && !t[y][x + 1]) t[y][++x] = tct;
			}
			else if (drc == 2) {
				while (x > 0 && !t[y][x - 1]) t[y][--x] = tct;
			}
			else if (drc == 3) {
				while (y > 0 && !t[y - 1][x]) t[--y][x] = tct;
			}
			else if (drc == 4) {
				while (y < h - 1 && !t[y + 1][x]) t[++y][x] = tct;
			}
		}

		int cnt = 0;
		for (int i = 0; i < 4; i++) {
			int nx = x + dx[i];
			int ny = y + dy[i];

			if ((nx >= 0 && nx < w) && (ny >= 0 && ny < h) && !t[ny][nx]) {
				if (i == 0) { //오른쪽
					s[k++].init(nx, ny, 1, tct + 1);
					cnt++;
				}
				else if (i == 1) { //아래쪽
					s[k++].init(nx, ny, 4, tct + 1);
					cnt++;
				}
				else if (i == 2) { //왼쪽
					s[k++].init(nx, ny, 2, tct + 1);
					cnt++;
				}
				else { //위쪽
					s[k++].init(nx, ny, 3, tct + 1);
					cnt++;
				}
			}
		}

		if (explore(t)) return 1;

		if (!cnt) {
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					if (k == 0 && t[i][j] != 1) t[i][j] = 0;
					else if (k && t[i][j] > s[k - 1].getCount() - 1) t[i][j] = 0;
				}
			}
		}
	}

	return 0;
}
int explore(int t[][WIDTH])
{
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (t[i][j] == 0) return 0;
		}
	}
	return 1;
}