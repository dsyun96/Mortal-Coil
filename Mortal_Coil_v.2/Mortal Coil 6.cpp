#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <Windows.h>
#include <stack>
#include <vector>
#include <string>
#include <queue>
#define X first
#define Y second
#define ABS(n) ((n) > 0 ? (n) : -(n))
using namespace std;

void display();
void backTurn(int num, int x, int y);
void dfs(int x, int y);
int bfs(int x, int y);
inline bool isValidIndex(int x, int y);
int dirDeadEnd(int x, int y);

//debug function
void printBoard();

int cntWhite, width, height;
bool isSolved = false;
char **board, **tmp;
string qpath;
stack<pair<int, int>> posPath;
stack<pair<int, int>> posDeadEnd;
const char dirStr[] = "ULRD";
const int dx[] = { -1, 0, 0, 1 };
const int dy[] = { 0, -1, 1, 0 };
const int around[4][2] = {
	{ 1, 2 },{ 0, 3 },{ 0, 3 },{ 1, 2 }
};

int main()
{
	//print interface in console
	display();

	//receive input data
	const int INPUT_SIZE = 30000;
	int pos = 0;
	char input[INPUT_SIZE];
	FILE *ifp = fopen("input.txt", "r");

	fscanf(ifp, "x=%d&y=%d&board=%s", &width, &height, input);

	board = new char*[height];
	tmp = new char*[height];
	for (int i = 0; i < height; ++i)
	{
		board[i] = new char[width];
		tmp[i] = new char[width];
		for (int j = 0; j < width; ++j)
		{
			board[i][j] = input[pos];
			cntWhite += input[pos++] == '.';
		}
	}

	//initialize count of dead end
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (board[i][j] == 'X') continue;
			if (dirDeadEnd(i, j) >= 0) posDeadEnd.push({ i, j });
		}
	}

	//brute force with dfs
	clock_t start = clock();
	int ansX, ansY;
	for (int j = 0; j < width; ++j)
	{
		for (int i = 0; i < height; ++i)
		{
			if (board[i][j] != '.') continue;
			clock_t substart = clock();

			--cntWhite;
			board[i][j] = '$';
			posPath.push({ i, j });
			dfs(i, j);
			if (isSolved)
			{
				ansX = i;
				ansY = j;
				goto EXIT;
			}
			printf("(%2d,%2d) passed (%.3f seconds)\n", j, i, (clock() - substart) / 1000.0);
			posPath.pop();
			board[i][j] = '.';
			++cntWhite;
		}
	}

EXIT:;
	printf("\a%.3f seconds\n", (clock() - start) / 1000.0);
	printf("http://www.hacker.org/coil/index.php?x=%d&y=%d&qpath=%s\n\n", ansY, ansX, qpath.c_str());

	//file close
	fclose(ifp);

	//delete dynamic memory allocated
	for (int i = 0; i < height; ++i)
	{
		delete[] board[i];
		delete[] tmp[i];
	}
	delete[] board;
	delete[] tmp;

	system("pause");

	return 0;
}

void display()
{
	//system("mode con: lines=40 cols=130");
	system("title Mortal Coil Solution v.1.95");

	puts("");
	puts("  _|      _|                        _|                _|        _|_|_|            _|  _|\n"
		"  _|_|  _|_|    _|_|    _|  _|_|  _|_|_|_|    _|_|_|  _|      _|          _|_|        _|\n"
		"  _|  _|  _|  _|    _|  _|_|        _|      _|    _|  _|      _|        _|    _|  _|  _|\n"
		"  _|      _|  _|    _|  _|          _|      _|    _|  _|      _|        _|    _|  _|  _|\n"
		"  _|      _|    _|_|    _|            _|_|    _|_|_|  _|        _|_|_|    _|_|    _|  _|\n\n");

	puts("    _|_|_|            _|              _|      _|                    \n"
		"  _|          _|_|    _|  _|    _|  _|_|_|_|        _|_|    _|_|_|  \n"
		"    _|_|    _|    _|  _|  _|    _|    _|      _|  _|    _|  _|    _|\n"
		"        _|  _|    _|  _|  _|    _|    _|      _|  _|    _|  _|    _|\n"
		"  _|_|_|      _|_|    _|    _|_|_|      _|_|  _|    _|_|    _|    _|  v.1.95\n\n");

	printf("* Please enter flashvars element of html embed tag.\n\n");
	printf("* For example: x=5&y=5&board=X......XX................\n\n");
	printf(">>> ");
}
void dfs(int x, int y)
{
	bool isConnected;
	for (int i = 0; i < 4; ++i)
	{
		if (isSolved) return;
		int nx = x + dx[i];
		int ny = y + dy[i];

		if (!isValidIndex(nx, ny) || board[nx][ny] != '.') continue;

		for (int j = 0; j < 4; ++j)
		{
			if (j == i) continue;
			int tx = x + dx[j];
			int ty = y + dy[j];

			if (isValidIndex(tx, ty) && board[tx][ty] == '.' && dirDeadEnd(tx, ty) >= 0) posDeadEnd.push({ tx, ty });
		}

		//ready to direction - i
		qpath.push_back(dirStr[i]);
		int cntTurn = 0;

		int dir = i;
		while (1)
		{
			do
			{
				--cntWhite;
				board[nx][ny] = '@';

				if (isValidIndex(nx + dx[dir], ny + dy[dir]) &&
					board[nx + dx[dir]][ny + dy[dir]] == '.')
				{
					for (int j = 0; j < 2; ++j)
					{
						int tx = nx + dx[around[dir][j]];
						int ty = ny + dy[around[dir][j]];

						if (!isValidIndex(tx, ty) || board[tx][ty] != '.') continue;
						if (dirDeadEnd(tx, ty) >= 0) posDeadEnd.push({ tx, ty });
					}
				}

				nx += dx[dir];
				ny += dy[dir];
			} while (isValidIndex(nx, ny) && board[nx][ny] == '.');

			if (cntWhite == 0)
			{
				isSolved = true;
				return;
			}

			nx -= dx[dir];
			ny -= dy[dir];

			posPath.push({ nx, ny });
			++cntTurn;
			if (posDeadEnd.size() >= 2) goto EXIT;

			dir = dirDeadEnd(nx, ny);
			//printBoard();
			if (dir < 0) break;

			nx += dx[dir];
			ny += dy[dir];
		}

		isConnected = false;
		for (int j = 0; j < 4; ++j)
		{
			int tx = nx + dx[j];
			int ty = ny + dy[j];

			if (isValidIndex(tx, ty) && board[tx][ty] == '.')
			{
				isConnected = bfs(tx, ty);
				break;
			}
		}

		//printf("%s\n", qpath.c_str());
		if (isConnected) dfs(nx, ny);
		if (isSolved) return;

	EXIT:;
		//printBoard();
		backTurn(cntTurn, nx, ny);
		qpath.pop_back();
		//printBoard();
	}
}
void backTurn(int num, int x, int y)
{
	int dir;
	while (num--)
	{
		posPath.pop();
		auto now = posPath.top();

		if (now.X > x) dir = 3;
		else if (now.X < x) dir = 0;

		if (now.Y > y) dir = 2;
		else if (now.Y < y) dir = 1;

		while (now.X != x || now.Y != y)
		{
			board[x][y] = '.';
			++cntWhite;

			x += dx[dir];
			y += dy[dir];
		}

		while (!posDeadEnd.empty())
		{
			auto now = posDeadEnd.top();
			if (ABS(now.X - x) + ABS(now.Y - y) >= 2 && dirDeadEnd(now.X, now.Y) < 0 ||
				ABS(now.X - x) + ABS(now.Y - y) < 2 && dirDeadEnd(now.X, now.Y) >= 0) posDeadEnd.pop();
			else break;
		}
	}
}
int bfs(int x, int y)
{
	for (int i = 0; i < height; ++i) memcpy(tmp[i], board[i], width);

	int ret = 0;
	queue<pair<int, int>> q;

	q.push({ x, y });
	tmp[x][y] = 'X';
	while (!q.empty())
	{
		auto now = q.front(); q.pop();
		++ret;

		for (int i = 0; i < 4; ++i)
		{
			int nx = now.X + dx[i];
			int ny = now.Y + dy[i];

			if (!isValidIndex(nx, ny) || tmp[nx][ny] != '.') continue;
			tmp[nx][ny] = 'X';
			q.push({ nx, ny });
		}
	}

	return ret == cntWhite;
}
inline bool isValidIndex(int x, int y)
{
	return x >= 0 && x < height && y >= 0 && y < width;
}
int dirDeadEnd(int x, int y)
{
	int ret = 0, idx;
	for (int i = 0; i < 4; ++i)
	{
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (isValidIndex(nx, ny) && board[nx][ny] == '.')
		{
			++ret;
			idx = i;
		}
	}

	return ret == 1 ? idx : -1;
}
void printBoard()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j) putchar(board[i][j]);
		puts("");
	}
	puts("");
}