#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <Windows.h>
#include <stack>
#include <queue>
#include <vector>
#include <string>
#define X first
#define Y second
#define ABS(n) ((n) > 0 ? (n) : -(n))
using namespace std;

void display();
void backTurn(int num, int x, int y);
void dfs(int x, int y);
//int bfs(int x, int y);
bool trace(int x, int y, int turn);
//please be careful to use
void nextPos(int &x, int &y, int &dir, int &wall);
void nextBF(int &x, int &y, int &bw, int &bh, int &ew, int &eh);
void restore(int x, int y);
inline bool isValidIndex(int x, int y);
inline bool isValidPos(int x, int y);
int dirDeadEnd(int x, int y);

//debug function
void printBoard(int len = 0, int x = -1, int y = -1);

int cntWhite, width, height;
bool isSolved = false;
char **board;
string qpath;
vector<pair<int, int>> posCorner;
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

	if (ifp == nullptr)
	{
		printf("File open failed.\n");
		system("pause");
		return 0;
	}
	fscanf(ifp, "x=%d&y=%d&board=%s", &width, &height, input);

	board = new char*[height];
	for (int i = 0; i < height; ++i)
	{
		board[i] = new char[width];
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
	int j = 0, i = 0, ansX, ansY;
	int bw = 0, bh = 0;
	int ew = width - 1, eh = height - 1;

	while (1)
	{
		while (board[i][j] != '.') nextBF(i, j, bw, bh, ew, eh);
		clock_t substart = clock();

		--cntWhite;
		board[i][j] = '@';
		posCorner.push_back({ i, j });
		dfs(i, j);
		if (isSolved)
		{
			ansX = i;
			ansY = j;
			goto EXIT;
		}
		printf("(%2d,%2d) passed (%.3f seconds)\n", j, i, (clock() - substart) / 1000.0);
		posCorner.pop_back();
		board[i][j] = '.';
		++cntWhite;

		nextBF(i, j, bw, bh, ew, eh);
	}

EXIT:;
	printf("\a%.3f seconds\n", (clock() - start) / 1000.0);
	printf("http://www.hacker.org/coil/index.php?x=%d&y=%d&qpath=%s\n\n", ansY, ansX, qpath.c_str());

	//file close
	fclose(ifp);

	//delete dynamic memory allocated
	for (int i = 0; i < height; ++i) delete[] board[i];
	delete[] board;

	system("pause");

	return 0;
}

void display()
{
	//system("mode con: lines=40 cols=130");
	system("title Mortal Coil Solution v.2.1.8");

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
		"  _|_|_|      _|_|    _|    _|_|_|      _|_|  _|    _|_|    _|    _|  v.2.1.8\n\n");

	printf("* Please put \"input.txt\" file that has flashvars element of html embed tag.\n\n");
	printf("* For example: x=5&y=5&board=X......XX................\n\n");
	//printf(">>> ");
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

		//ready to direction - i
		//check whether there is dead end around start position or not
		for (int j = 0; j < 4; ++j)
		{
			if (j == i) continue;
			int tx = x + dx[j];
			int ty = y + dy[j];

			if (isValidIndex(tx, ty) && board[tx][ty] == '.' && dirDeadEnd(tx, ty) >= 0) posDeadEnd.push({ tx, ty });
		}

		qpath.push_back(dirStr[i]);
		int cntTurn = 0;

		int dir = i;
		while (1)
		{
			do
			{
				--cntWhite;
				board[nx][ny] = '@';

				//check dead end
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

			posCorner.push_back({ nx, ny });
			++cntTurn;
			if (posDeadEnd.size() >= 2) goto EXIT;

			dir = dirDeadEnd(nx, ny);
			//printBoard(12);
			if (dir < 0) break;

			nx += dx[dir];
			ny += dy[dir];
		}

		isConnected = trace(nx, ny, cntTurn);

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
		posCorner.pop_back();
		auto now = posCorner.back();

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
/*
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
*/
bool trace(int x, int y, int turn)
{
	/*
	int sz = posCorner.size();
	const auto a = posCorner[sz - 2];
	int px = a.X, py = a.Y;
	int cx = x, cy = y;
	int aroundWhite = 0;

	//count white tile around both end points
	int nx, ny;
	for (int i = 0; i < 4; ++i)
	{
	nx = px + dx[i];
	ny = py + dy[i];

	if (isValidIndex(nx, ny) && board[nx][ny] == '.') ++aroundWhite;

	nx = x + dx[i];
	ny = y + dy[i];

	if (isValidIndex(nx, ny) && board[nx][ny] == '.') ++aroundWhite;
	}

	//count white tile around others
	while (1)
	{
	px += dx[dir];
	py += dy[dir];
	if (px == x && py == y) break;

	for (int i = 0; i < 2; ++i)
	{
	nx = px + dx[around[dir][i]];
	ny = py + dy[around[dir][i]];

	if (!isValidIndex(nx, ny) || board[nx][ny] != '.') continue;
	++aroundWhite;
	}
	}

	//initialize start position
	nx = x, ny = y;
	int subdir;
	for (int i = 0; i < 2; ++i)
	{
	int tx = x + dx[around[dir][i]];
	int ty = y + dy[around[dir][i]];

	if (!isValidIndex(tx, ty) || board[tx][ty] != '.') continue;

	x = tx;
	y = ty;
	subdir = around[dir][i];
	break;
	}

	//in case that getting stuck
	if (x == nx && y == ny) return false;

	//follow a wall
	--aroundWhite;

	int beginx = x, beginy = y, nowdir = dir ^ 3, wall = subdir ^ 3;
	int range = dir == 0 or dir == 3;

	printBoard(x, y);
	*/

	int nowIdx = posCorner.size() - 1 - turn;
	auto nowPos = posCorner[nowIdx];
	int aroundWhite = 0, dir;

	while (nowIdx + 1 < posCorner.size())
	{
		auto nextPos = posCorner[++nowIdx];

		if (nowPos.X == nextPos.X)
		{
			if (nowPos.Y < nextPos.Y) dir = 2;
			else dir = 1;
		}
		else
		{
			if (nowPos.X < nextPos.X) dir = 3;
			else dir = 0;
		}

		do
		{
			board[nowPos.X][nowPos.Y] = '#';
			for (int i = 0; i < 4; ++i)
			{
				int nx = nowPos.X + dx[i];
				int ny = nowPos.Y + dy[i];

				if (!isValidIndex(nx, ny) || board[nx][ny] != '.') continue;

				board[nx][ny] = '*';
				++aroundWhite;
			}

			nowPos.X += dx[dir];
			nowPos.Y += dy[dir];
		} while (nowPos.X != nextPos.X || nowPos.Y != nextPos.Y);
	}

	int nowdir = dir ^ 3;
	int wall = -1;
	for (int i = 0; i < 4; ++i)
	{
		int nx = nowPos.X + dx[i];
		int ny = nowPos.Y + dy[i];

		if (!isValidIndex(nx, ny) || !isValidPos(nx, ny)) continue;
		if (board[nx][ny] == '.')
		{
			board[nx][ny] = '*';
			++aroundWhite;
		}
		x = nx;
		y = ny;
		wall = i ^ 3;
	}

	//printBoard(12);

	//in case that getting stuck
	if (wall < 0)
	{
		restore(nowPos.X, nowPos.Y);
		return false;
	}

	--aroundWhite;
	board[x][y] = '.';

	int bx = x;
	int by = y;
	int returnToBegin = 0;
	while (aroundWhite)
	{
		nextPos(x, y, nowdir, wall);
		//printBoard(12, x, y);
		if (x == bx && y == by) ++returnToBegin;
		if (returnToBegin > 1) break;

		if (board[x][y] == '*')
		{
			--aroundWhite;
			board[x][y] = '.';
		}
	}

	nowPos = posCorner.back();
	restore(nowPos.X, nowPos.Y);

	//printBoard();

	if (!aroundWhite) return true;
	else return false;
}
void nextPos(int &x, int &y, int &dir, int &wall)
{
	int tx = x + dx[wall];
	int ty = y + dy[wall];
	int tmp;

	//arive end of wall
	if (isValidIndex(tx, ty) && isValidPos(tx, ty))
	{
		x = tx;
		y = ty;

		tmp = dir ^ 3;
		dir = wall;
		wall = tmp;
		return;
	}

	//didn't arive end of wall yet
	tx = x + dx[dir];
	ty = y + dy[dir];

	//can go ahead to dir
	if (isValidIndex(tx, ty) && isValidPos(tx, ty))
	{
		x = tx;
		y = ty;
		return;
	}

	//can't go ahead to dir
	tx = x + dx[wall ^ 3];
	ty = y + dy[wall ^ 3];

	//a step backward to take two steps forward
	if (isValidIndex(tx, ty) && isValidPos(tx, ty))
	{
		x = tx;
		y = ty;

		tmp = wall ^ 3;
		wall = dir;
		dir = tmp;
		return;
	}

	//wtf? got stuck? holy...
	tx = x + dx[dir ^ 3];
	ty = y + dy[dir ^ 3];

	//if this condition doesn't true, that case is impossible
	if (isValidIndex(tx, ty) && isValidPos(tx, ty))
	{
		x = tx;
		y = ty;

		dir ^= 3;
		wall ^= 3;
		return;
	}

	/*********************************************************
	*
	*                       FUCK YOU
	*
	**********************************************************/
}
void nextBF(int &x, int &y, int &bw, int &bh, int &ew, int &eh)
{
	if (x == bh)
	{
		if (y == ew) ++x;
		else ++y;
	}
	else if (y == ew)
	{
		if (x == eh) --y;
		else ++x;
	}
	else if (x == eh)
	{
		if (y == bw) --x;
		else --y;
	}
	else if (y == bw)
	{
		if (x - 1 == bh)
		{
			++y;
			++bw; ++bh;
			--ew; --eh;
		}
		else --x;
	}
}
void restore(int x, int y)
{
	queue<pair<int, int>> q;
	q.push({ x, y });
	board[x][y] = '@';
	while (!q.empty())
	{
		auto now = q.front();
		q.pop();

		for (int i = 0; i < 4; ++i)
		{
			int nx = now.X + dx[i];
			int ny = now.Y + dy[i];

			if (!isValidIndex(nx, ny)) continue;

			if (board[nx][ny] == '*') board[nx][ny] = '.';
			else if (board[nx][ny] == '#') board[nx][ny] = '@', q.push({ nx, ny });
		}
	}
}
inline bool isValidIndex(int x, int y)
{
	return x >= 0 && x < height && y >= 0 && y < width;
}
inline bool isValidPos(int x, int y)
{
	return board[x][y] == '.' || board[x][y] == '*';
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
void printBoard(int len, int x, int y)
{
	if (len > qpath.size()) return;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j) putchar(x == i && y == j ? '_' : board[i][j]);
		puts("");
	}
	puts("");
}