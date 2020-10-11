#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <chrono>
#include <iomanip>
#include <Windows.h>
using namespace std;

void gotoxy(int y, int x) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}
// 10: grass
// 7 : white
// 6 : yellow
// 4 : red
// 9 : sky
void change_color(int n) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}

//#define PRINT_BOARD
#define X first
#define Y second
#define sz(x) (int)x.size()
#define all(x) x.begin(), x.end()

// . = can go
// X = can't go
// @ = went

const int dx[] = { -1, 0, 1, 0 };
const int dy[] = { 0, 1, 0, -1 };
const char* dir_mark[] = { "¡è", "¡æ", "¡é", "¡ç" };
int R, C;
int total_cell;
int dead_end;

bool is_valid(int x, int y) {
	return 0 <= x && x < R && 0 <= y && y < C;
}
bool is_dead_end(const vector<vector<char>>& bo, int x, int y) {
	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (is_valid(nx, ny) && bo[nx][ny] == '.') ++cnt;
	}
	return cnt == 1;
}
bool is_separate(const vector<vector<char>>& bo, int start_x, int start_y, int start_dir) {
	int x = start_x + dx[(start_dir + 1) % 4];
	int y = start_y + dy[(start_dir + 1) % 4];
	int dir = (start_dir + 3) % 4;
	if (!is_valid(x, y) || bo[x][y] != '.') return false;

	int opp_x = x + dx[dir] * 2;
	int opp_y = y + dy[dir] * 2;
	if (!is_valid(opp_x, opp_y) || bo[opp_x][opp_y] != '.') return false;

	for (int now_x = x, now_y = y, now_dir = dir; now_x != opp_x || now_y != opp_y; ) {
		for (int i = 0; i < 4; ++i) {
			int nxt_x = now_x + dx[(now_dir + 1) % 4];
			int nxt_y = now_y + dy[(now_dir + 1) % 4];
			if (is_valid(nxt_x, nxt_y) && bo[nxt_x][nxt_y] == '.') {
				now_x = nxt_x;
				now_y = nxt_y;
				if (now_x == x && now_y == y && now_dir == dir) return true;
				
				int nxt_nxt_x = nxt_x + dx[now_dir % 4];
				int nxt_nxt_y = nxt_y + dy[now_dir % 4];
				if (is_valid(nxt_nxt_x, nxt_nxt_y) && bo[nxt_nxt_x][nxt_nxt_y] == '.') now_dir = (now_dir + 3) % 4;

				break;
			}
			now_dir = (now_dir + 1) % 4;
			if (now_x == x && now_y == y && now_dir == dir) return true;
		}
	}

	return false;
}
bool just_go(vector<vector<char>>& bo, vector<pair<int, int>>& walks, int& x, int& y, int& dir) {
	while (1) {
		while (1) {
			int nxt_x = x + dx[dir];
			int nxt_y = y + dy[dir];
			if (is_valid(nxt_x, nxt_y) && bo[nxt_x][nxt_y] == '.') {
				bo[nxt_x][nxt_y] = '@';
				--total_cell;

				for (int i = 0; i < 4; ++i) {
					int nx = x + dx[i];
					int ny = y + dy[i];
					if (!is_valid(nx, ny) || bo[nx][ny] != '.') continue;
					if (is_dead_end(bo, nx, ny)) ++dead_end;
				}

				x = nxt_x;
				y = nxt_y;
#ifdef PRINT_BOARD
				gotoxy(x, y * 2);
				change_color(6);
				cout << dir_mark[dir];
#endif
				walks.emplace_back(x, y);
				if (is_separate(bo, x, y, dir) || dead_end > 1) return true;
			}
			else break;
		}
		
		bool two_selections = true;
		int nxt_dir = -1;
		for (int i = 1; i < 4; i += 2) {
			int nxt_x = x + dx[(dir + i) % 4];
			int nxt_y = y + dy[(dir + i) % 4];
			if (!is_valid(nxt_x, nxt_y) || bo[nxt_x][nxt_y] != '.') two_selections = false;
			else nxt_dir = (dir + i) % 4;
		}

		if (two_selections || nxt_dir == -1) break;
		dir = nxt_dir;
	}

	return false;
}
string brute_force(vector<vector<char>> bo, int start_x, int start_y) {
	vector<tuple<int, int, int, int>> trace;
	vector<vector<pair<int, int>>> roll_back;

	for (int i = 0; i < 4; ++i) {
		int nx = start_x + dx[i];
		int ny = start_y + dy[i];
		if (!is_valid(nx, ny) || bo[nx][ny] != '.') continue;
		trace.emplace_back(start_x, start_y, i, 1);
	}

	if (is_dead_end(bo, start_x, start_y)) --dead_end;

#ifdef PRINT_BOARD
	gotoxy(start_x, start_y * 2);
	change_color(4);
	cout << "¡á";
#endif
	bo[start_x][start_y] = '@';
	--total_cell;

	string qpath;
	while (!trace.empty()) {
		auto [x, y, dir, num] = trace.back();
		trace.pop_back();

		while (num <= sz(roll_back)) {
			auto& now = roll_back.back();
			for (int i = sz(now); --i; ) {
				auto [x, y] = now[i - 1];
				for (int j = 0; j < 4; ++j) {
					int nx = x + dx[j];
					int ny = y + dy[j];
					if (!is_valid(nx, ny) || bo[nx][ny] != '.') continue;
					if (is_dead_end(bo, nx, ny)) --dead_end;
				}

#ifdef PRINT_BOARD
				gotoxy(now[i].X, now[i].Y * 2);
				change_color(7);
				cout << "¡á";
#endif
				bo[now[i].X][now[i].Y] = '.';
				++total_cell;
			}
			roll_back.pop_back();
			qpath.pop_back();
		}
		qpath += "URDL"[dir];

		vector<pair<int, int>> walks;
		walks.emplace_back(x, y);
		bool sep = just_go(bo, walks, x, y, dir);
		roll_back.emplace_back(walks);

		if (dead_end > 1 || sep) continue;
		if (total_cell == 0) return qpath;

		for (int i = 1; i < 4; i += 2) {
			int nxt_x = x + dx[(dir + i) % 4];
			int nxt_y = y + dy[(dir + i) % 4];
			if (is_valid(nxt_x, nxt_y) && bo[nxt_x][nxt_y] == '.') trace.emplace_back(x, y, (dir + i) % 4, num + 1);
		}
	}

	while (!roll_back.empty()) {
		auto& now = roll_back.back();
		for (int i = sz(now); --i; ) {
			auto [x, y] = now[i - 1];
			for (int j = 0; j < 4; ++j) {
				int nx = x + dx[j];
				int ny = y + dy[j];
				if (!is_valid(nx, ny) || bo[nx][ny] != '.') continue;
				if (is_dead_end(bo, nx, ny)) --dead_end;
			}

#ifdef PRINT_BOARD
			gotoxy(now[i].X, now[i].Y * 2);
			change_color(7);
			cout << "¡á";
#endif
			bo[now[i].X][now[i].Y] = '.';
			++total_cell;
		}
		roll_back.pop_back();
	}

#ifdef PRINT_BOARD
	gotoxy(start_x, start_y * 2);
	change_color(7);
	cout << "¡á";
#endif
	bo[start_x][start_y] = '.';
	++total_cell;

	if (is_dead_end(bo, start_x, start_y)) ++dead_end;

	return "";
}
int main() {
	cin.sync_with_stdio(false);
	cin.tie(nullptr);
	cout << fixed << setprecision(3);

	int bo_y, bo_x;
	string bo_info;
	ifstream info("board_info.txt");
	info >> bo_y >> bo_x >> bo_info;
	info.close();

	R = bo_x;
	C = bo_y;

	vector<vector<char>> bo(bo_x, vector<char>(bo_y));
	for (int idx = 0, i = 0; i < bo_x; ++i) {
		for (int j = 0; j < bo_y; ++j) {
			bo[i][j] = bo_info[idx++];
			if (bo[i][j] == '.') ++total_cell;
		}
	}

	for (int i = 0; i < bo_x; ++i) {
		for (int j = 0; j < bo_y; ++j) {
			if (bo[i][j] != '.') continue;
			if (is_dead_end(bo, i, j)) ++dead_end;
		}
	}

#ifdef PRINT_BOARD
	for (auto& i : bo) {
		for (char j : i) {
			if (j == 'X') change_color(10);
			else if (j == '.') change_color(7);
			cout << "¡á";
		}
		cout << endl;
	}
#endif

	double total_time = 0;
	for (int i = 0; i < bo_x; ++i) {
		for (int j = 0; j < bo_y; ++j) {
			if (bo[i][j] == 'X') continue;

			// begin brute forcing from (i, j)
			chrono::system_clock::time_point start = chrono::system_clock::now();

#ifndef PRINT_BOARD
			cout << "(" << i << ", " << j << ") ";
#endif
			string res = brute_force(bo, i, j);

			chrono::duration<double> sec = chrono::system_clock::now() - start;
			total_time += sec.count();

#ifndef PRINT_BOARD
			cout << sec.count() << " seconds\n";
#endif
			if (res != "") {
#ifdef PRINT_BOARD
				gotoxy(bo_x, 0);
				change_color(7);
#endif
				cout << "\nFINISHED!\nIt takes " << total_time << " seconds.\n\n";

				ofstream out("outurl.txt");
				out << "http://www.hacker.org/coil/index.php?x=" << j << "&y=" << i << "&qpath=" << res;
				out.close();

				return 0;
			}
		}
	}

	return 0;
}