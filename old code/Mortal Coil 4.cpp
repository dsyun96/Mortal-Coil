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
using namespace std;

void display();
void backTurn(int num, int x, int y);
void dfs(int x, int y);
int bfs(int x, int y);
inline bool isValidIndex(int x, int y);
int dirDeadEnd(int x, int y);

//debug function
void printBoard();

int cntWhite, width, height, cntDeadEnd;
bool isSolved = false;
char **board;
string qpath;
stack<pair<int, int>> posPath;
const char dirStr[] = "ULRD";
const int dx[] = { -1, 0, 0, 1 };
const int dy[] = { 0, -1, 1, 0 };

int main()
{
	//print interface in console
	display();

	//receive input data
	const int INPUT_SIZE = 1000;
	int pos = 0;
	char input[INPUT_SIZE];

	scanf("x=%d&y=%d&board=%s", &width, &height, input);

	board = new char*[height];
	for (int i = 0; i < height; ++i)
	{
		board[i] = new char[width + 1];
		for (int j = 0; j < width; ++j)
		{
			board[i][j] = input[pos];
			cntWhite += input[pos++] == '.';
		}
		board[i][width] = 0;
	}

	//initialize count of dead end
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (board[i][j] == 'X') continue;
			if (dirDeadEnd(i, j)) ++cntDeadEnd;
		}
	}

	//brute force with dfs
	clock_t start = clock();
	int ansX, ansY;
	for (int j = 0; j < width; ++j)
	{
		for (int i = 0; i < height; ++i)
		{
			if (board[i][j] == 'X') continue;
			clock_t substart = clock();

			--cntWhite;
			board[i][j] = 'X';
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
	printf("%.3f seconds\n", (clock() - start) / 1000.0);
	printf("http://www.hacker.org/coil/index.php?x=%d&y=%d&qpath=%s\n\n", ansY, ansX, qpath.c_str());

	//delete dynamic memory allocated
	for (int i = 0; i < height; ++i) delete[] board[i];
	delete[] board;

	system("pause");

	return 0;
}

void display()
{
	//system("mode con: lines=40 cols=130");
	system("title Mortal Coil Solution v.1.43");

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
		 "  _|_|_|      _|_|    _|    _|_|_|      _|_|  _|    _|_|    _|    _|  v.1.43\n\n");

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

		if (!isValidIndex(nx, ny) || board[nx][ny] == 'X') continue;

		//ready to direction - i
		qpath.push_back(dirStr[i]);
		int cntTurn = 0;

		int dir = i;
		while (1)
		{
			do
			{
				--cntWhite;
				board[nx][ny] = 'X';

				//if (isValidIndex(nx + dx[dir], ny + dy[dir]) &&
				//	board[nx + dx[dir]][ny + dy[dir]] != 'X')
				//{
				//	for (int j = 0; j < 4; ++j)
				//	{
				//		if (j == dir) continue;
				//		int tx = nx + dx[j];
				//		int ty = ny + dy[j];
				//
				//		if (!isValidIndex(tx, ty) || board[tx][ty] == 'X') continue;
				//		if (dirDeadEnd(tx, ty) >= 0) ++cntDeadEnd;
				//	}
				//}
				//
				//if (cntDeadEnd >= 2)
				//{
				//	posPath.push({ nx, ny });
				//	++cntTurn;
				//	goto EXIT;
				//}

				nx += dx[dir];
				ny += dy[dir];
			} while (isValidIndex(nx, ny) && board[nx][ny] != 'X');

			if (cntWhite == 0)
			{
				isSolved = true;
				return;
			}

			nx -= dx[dir];
			ny -= dy[dir];

			posPath.push({ nx, ny });
			++cntTurn;
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
		backTurn(cntTurn, nx, ny);
		qpath.pop_back();
	}
}
void backTurn(int num, int x, int y)
{
	while (num--)
	{
		posPath.pop();
		auto now = posPath.top();

		while (now.X != x || now.Y != y)
		{
			//for (int i = 0; i < 4; ++i)
			//{
			//	int nx = x + dx[i];
			//	int ny = y + dy[i];
			//
			//	if (!isValidIndex(nx, ny) || board[nx][ny] == 'X') continue;
			//	if (dirDeadEnd(nx, ny) >= 0) --cntDeadEnd;
			//}

			board[x][y] = '.';
			++cntWhite;

			if (now.X > x) ++x;
			else if (now.X < x) --x;

			if (now.Y > y) ++y;
			else if (now.Y < y) --y;
		}
	}
}
int bfs(int x, int y)
{
	char **tmp = new char*[height];
	for (int i = 0; i < height; ++i)
	{
		tmp[i] = new char[width + 1];
		memcpy(tmp[i], board[i], width + 1);
	}

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

			if (!isValidIndex(nx, ny) || tmp[nx][ny] == 'X') continue;
			tmp[nx][ny] = 'X';
			q.push({ nx, ny });
		}
	}
	
	for (int i = 0; i < height; ++i) delete[] tmp[i];
	delete[] tmp;

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