#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
#include <Windows.h>
void gotoxy(int y, int x) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}
// 10: grass
// 7 : white
// 6 : yellow
// 4 : red
// 9 : sky
// 13: pink
void change_color(int n) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}
#endif

//#define PRINT_BOARD
#define PRINT_SEPARATE
#define PRINT_DEAD_END
#define PRINT_GO
#define X first
#define Y second
#define sz(x) (int)x.size()
#define all(x) x.begin(), x.end()

using namespace std;

// . = can go
// X = can't go
// @ = went

const int dx[] = { -1, 0, 1, 0 };
const int dy[] = { 0, 1, 0, -1 };
const char* dir_mark[] = { "бш", "бц", "бщ", "бч" };
int R, C;
int total_cell;
vector<pair<int, int>> dead_ends;

bool is_valid(int x, int y) {
	return 0 <= x && x < R && 0 <= y && y < C;
}
bool is_dead_end(const vector<vector<tuple<char, int, int>>>& bo, int x, int y) {
	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (is_valid(nx, ny) && get<0>(bo[nx][ny]) == '.') ++cnt;
	}
	return cnt == 1;
}
bool is_separate(const vector<vector<tuple<char, int, int>>>& bo, int start_x, int start_y, int start_dir) {
	/*int x = start_x + dx[(start_dir + 1) % 4];
	int y = start_y + dy[(start_dir + 1) % 4];
	int left_hand_dir = (start_dir + 3) % 4;
	if (!is_valid(x, y) || get<0>(bo[x][y]) != '.') return false;*/

	int x = -1, y = -1, left_hand_dir = -1;
	vector<pair<int, int>> near_cells;
	for (int i = 0; i < 4; ++i) {
		int nx = start_x + dx[i];
		int ny = start_y + dy[i];
		if (!is_valid(nx, ny) || get<0>(bo[nx][ny]) != '.') continue;

		x = nx;
		y = ny;
		left_hand_dir = (i + 2) % 4;
		near_cells.emplace_back(nx, ny);
	}

	if (sz(near_cells) <= 1) return false;

#if defined(PRINT_BOARD) && defined(PRINT_SEPARATE)
	vector<pair<int, int>> walks;
#endif
	for (int now_x = x, now_y = y, now_left_hand_dir = left_hand_dir; ; ) {
		for (int i = 0; i < 4; ++i) {
			int nxt_x = now_x + dx[(now_left_hand_dir + 1) % 4];
			int nxt_y = now_y + dy[(now_left_hand_dir + 1) % 4];
			if (is_valid(nxt_x, nxt_y) && get<0>(bo[nxt_x][nxt_y]) == '.') {
				now_x = nxt_x;
				now_y = nxt_y;
#if defined(PRINT_BOARD) && defined(PRINT_SEPARATE)
				walks.emplace_back(now_x, now_y);
				gotoxy(now_x, now_y * 2);
				change_color(13);
				cout << dir_mark[(now_left_hand_dir + 1) % 4];
#endif
				if (abs(now_x - start_x) + abs(now_y - start_y) == 1) {
					auto it = find(all(near_cells), pair<int, int>(now_x, now_y));
					if (it != near_cells.end()) near_cells.erase(it);
				}
				if (now_x == x && now_y == y && now_left_hand_dir == left_hand_dir) goto EXIT;
				
				int void_x = now_x + dx[now_left_hand_dir];
				int void_y = now_y + dy[now_left_hand_dir];
				if (is_valid(void_x, void_y) && get<0>(bo[void_x][void_y]) == '.') now_left_hand_dir = (now_left_hand_dir + 3) % 4;

				break;
			}
			now_left_hand_dir = (now_left_hand_dir + 1) % 4;
			if (now_x == x && now_y == y && now_left_hand_dir == left_hand_dir) goto EXIT;
		}
	}

EXIT:;
#if defined(PRINT_BOARD) && defined(PRINT_SEPARATE)
	for (auto [x, y] : walks) {
		gotoxy(x, y * 2);
		change_color(7);
		cout << "бс";
	}
#endif

	return !near_cells.empty();
}
int general_dead_end_num(vector<vector<tuple<char, int, int>>>& bo, int start_x, int start_y, int start_dir) {
	/*int x = start_x + dx[start_dir];
	int y = start_y + dy[start_dir];
	int hand_dir = (start_dir + 2) % 4;
	int ret = 0;

	vector<pair<int, int>> walks;
	int result_left_x = -1, result_left_y = -1;
	for (int now_x = x, now_y = y, now_left_hand_dir = hand_dir; ; ) {
		for (int i = 0; i < 4; ++i) {
			int nxt_x = now_x + dx[(now_left_hand_dir + 1) % 4];
			int nxt_y = now_y + dy[(now_left_hand_dir + 1) % 4];
			if (is_valid(nxt_x, nxt_y) && (bo[nxt_x][nxt_y] == '*' || bo[nxt_x][nxt_y] == '.')) {
				if (bo[nxt_x][nxt_y] == '*') {
					if (find(all(dead_ends), pair<int, int>(now_x, now_y)) == dead_ends.end()) ++ret;
					result_left_x = nxt_x;
					result_left_y = nxt_y;
					goto LEFT_HAND_END;
				}

				now_x = nxt_x;
				now_y = nxt_y;
				bo[now_x][now_y] = '*';
				walks.emplace_back(now_x, now_y);
#ifdef PRINT_BOARD
				gotoxy(now_x, now_y * 2);
				change_color(9);
				cout << dir_mark[(now_left_hand_dir + 1) % 4];
#endif

				int nxt_nxt_x = nxt_x + dx[now_left_hand_dir % 4];
				int nxt_nxt_y = nxt_y + dy[now_left_hand_dir % 4];
				if (is_valid(nxt_nxt_x, nxt_nxt_y) && bo[nxt_nxt_x][nxt_nxt_y] == '.') now_left_hand_dir = (now_left_hand_dir + 3) % 4;

				break;
			}
			now_left_hand_dir = (now_left_hand_dir + 1) % 4;
		}
		if (now_x == x && now_y == y && now_left_hand_dir == hand_dir) break;
	}

LEFT_HAND_END:;
	for (auto [ix, iy] : walks) {
		bo[ix][iy] = '.';
#ifdef PRINT_BOARD
		gotoxy(ix, iy * 2);
		change_color(7);
		cout << "бс";
#endif
	}
	walks.clear();

	if (result_left_x == -1) return 0;

	int result_right_x = -1, result_right_y = -1;
	for (int now_x = x, now_y = y, now_right_hand_dir = hand_dir; ; ) {
		for (int i = 0; i < 4; ++i) {
			int nxt_x = now_x + dx[(now_right_hand_dir + 3) % 4];
			int nxt_y = now_y + dy[(now_right_hand_dir + 3) % 4];
			if (is_valid(nxt_x, nxt_y) && (bo[nxt_x][nxt_y] == '*' || bo[nxt_x][nxt_y] == '.')) {
				if (bo[nxt_x][nxt_y] == '*') {
					if (find(all(dead_ends), pair<int, int>(now_x, now_y)) == dead_ends.end()) ++ret;
					result_right_x = nxt_x;
					result_right_y = nxt_y;
					goto RIGHT_HAND_END;
				}

				now_x = nxt_x;
				now_y = nxt_y;
				bo[now_x][now_y] = '*';
				walks.emplace_back(now_x, now_y);
#ifdef PRINT_BOARD
				gotoxy(now_x, now_y * 2);
				change_color(9);
				cout << dir_mark[(now_right_hand_dir + 3) % 4];
#endif

				int nxt_nxt_x = nxt_x + dx[now_right_hand_dir % 4];
				int nxt_nxt_y = nxt_y + dy[now_right_hand_dir % 4];
				if (is_valid(nxt_nxt_x, nxt_nxt_y) && bo[nxt_nxt_x][nxt_nxt_y] == '.') now_right_hand_dir = (now_right_hand_dir + 1) % 4;

				break;
			}
			now_right_hand_dir = (now_right_hand_dir + 3) % 4;
		}
		if (now_x == x && now_y == y && now_right_hand_dir == hand_dir) break;
	}

RIGHT_HAND_END:;
	for (auto [ix, iy] : walks) {
		bo[ix][iy] = '.';
#ifdef PRINT_BOARD
		gotoxy(ix, iy * 2);
		change_color(7);
		cout << "бс";
#endif
	}
	walks.clear();

	if (result_left_x == result_right_x && result_left_y == result_right_y) --ret;
	return ret + sz(dead_ends);*/

	//start_x += dx[start_dir];
	//start_y += dy[start_dir];
	int hand_dir = (start_dir + 2) % 4;
	int num = 0, ret = sz(dead_ends);

	if (is_dead_end(bo, start_x, start_y)) get<0>(bo[start_x][start_y]) = '@';

	vector<pair<int, int>> walks, visits;
	for (int flag = 0, init_x = -1, init_y = -1, init_dir = -1, now_x = start_x, now_y = start_y, now_left_hand_dir = hand_dir; ; ) {
		int i;
		for (i = 0; i < 4; ++i) {
			int nxt_x = now_x + dx[(now_left_hand_dir + 1) % 4];
			int nxt_y = now_y + dy[(now_left_hand_dir + 1) % 4];
			if (is_valid(nxt_x, nxt_y) && get<0>(bo[nxt_x][nxt_y]) == '.') {
				if (flag == -1 && nxt_x == init_x && nxt_y == init_y) {
					if (!visits.empty()) {
						auto [x, y] = visits.back();
						if (x == get<1>(bo[now_x][now_y]) && y == get<2>(bo[now_x][now_y]))
							visits.pop_back();
					}
					goto EXIT;
				}

				walks.emplace_back(nxt_x, nxt_y);
#if defined(PRINT_BOARD) && defined(PRINT_DEAD_END)
				gotoxy(nxt_x, nxt_y * 2);
				change_color(9);
				cout << dir_mark[(now_left_hand_dir + 1) % 4];
#endif

				if (get<1>(bo[nxt_x][nxt_y]) == -1) get<1>(bo[nxt_x][nxt_y]) = num++;
				else {
					if (is_dead_end(bo, now_x, now_y) && find(all(dead_ends), pair<int, int>(now_x, now_y)) != dead_ends.end()) --ret;
					get<2>(bo[nxt_x][nxt_y]) = num++;
					if ((nxt_x != init_x || nxt_y != init_y) &&!visits.empty() && get<1>(bo[nxt_x][nxt_y]) < visits.back().X && visits.back().Y < get<2>(bo[nxt_x][nxt_y])) visits.pop_back();
					visits.emplace_back(get<1>(bo[nxt_x][nxt_y]), get<2>(bo[nxt_x][nxt_y]));
				}

				if (ret + sz(visits) > 2) goto EXIT;

				int void_x = nxt_x + dx[now_left_hand_dir];
				int void_y = nxt_y + dy[now_left_hand_dir];
				if (is_valid(void_x, void_y) && get<0>(bo[void_x][void_y]) == '.') now_left_hand_dir = (now_left_hand_dir + 3) % 4;

				if (flag == 0) {
					if (is_dead_end(bo, nxt_x, nxt_y)) get<0>(bo[nxt_x][nxt_y]) = '@';
					else {
						flag = 1;
						init_x = nxt_x;
						init_y = nxt_y;
						init_dir = now_left_hand_dir;
					}
				}
				else if (flag == 1) flag = -1;

				now_x = nxt_x;
				now_y = nxt_y;

				break;
			}
			now_left_hand_dir = (now_left_hand_dir + 1) % 4;
		}
		if (i == 4) break;
	}

EXIT:;
	for (auto [x, y] : walks) {
		get<1>(bo[x][y]) = get<2>(bo[x][y]) = -1;
		get<0>(bo[x][y]) = '.';
#if defined(PRINT_BOARD) && defined(PRINT_DEAD_END)
		gotoxy(x, y * 2);
		change_color(7);
		cout << "бс";
#endif
	}

	if (is_dead_end(bo, start_x, start_y)) get<0>(bo[start_x][start_y]) = '.';

	return ret + sz(visits);
}
bool just_go(vector<vector<tuple<char, int, int>>>& bo, vector<pair<int, int>>& walks, int& x, int& y, int& dir) {
	while (1) {
		while (1) {
			int nxt_x = x + dx[dir];
			int nxt_y = y + dy[dir];
			if (is_valid(nxt_x, nxt_y) && get<0>(bo[nxt_x][nxt_y]) == '.') {
				get<0>(bo[nxt_x][nxt_y]) = '@';
				--total_cell;

				for (int i = 0; i < 4; ++i) {
					int nx = x + dx[i];
					int ny = y + dy[i];
					if (!is_valid(nx, ny) || get<0>(bo[nx][ny]) != '.') continue;
					if (is_dead_end(bo, nx, ny)) dead_ends.emplace_back(nx, ny);
				}

				x = nxt_x;
				y = nxt_y;
#if defined(PRINT_BOARD) && defined(PRINT_GO)
				gotoxy(x, y * 2);
				change_color(6);
				cout << dir_mark[dir];
#endif
				walks.emplace_back(x, y);
				if (is_separate(bo, x, y, dir) || sz(dead_ends) > 1) return true;
			}
			else break;
		}
		
		bool two_selections = true;
		int nxt_dir = -1;
		for (int i = 1; i < 4; i += 2) {
			int nxt_x = x + dx[(dir + i) % 4];
			int nxt_y = y + dy[(dir + i) % 4];
			if (!is_valid(nxt_x, nxt_y) || get<0>(bo[nxt_x][nxt_y]) != '.') two_selections = false;
			else nxt_dir = (dir + i) % 4;
		}

		if (nxt_dir == -1) break;
		if (two_selections) return general_dead_end_num(bo, x, y, (dir + 1) % 4) > 1 && general_dead_end_num(bo, x, y, (dir + 3) % 4) > 1;

		dir = nxt_dir;
	}

	return false;
}
string brute_force(vector<vector<tuple<char, int, int>>>& bo, int start_x, int start_y) {
	vector<tuple<int, int, int, int>> trace;
	vector<vector<pair<int, int>>> roll_back;

	for (int i = 0; i < 4; ++i) {
		int nx = start_x + dx[i];
		int ny = start_y + dy[i];
		if (!is_valid(nx, ny) || get<0>(bo[nx][ny]) != '.') continue;
		trace.emplace_back(start_x, start_y, i, 1);
	}

	if (is_dead_end(bo, start_x, start_y)) dead_ends.erase(find(all(dead_ends), pair<int, int>(start_x, start_y)));

#if defined(PRINT_BOARD) && defined(PRINT_GO)
	gotoxy(start_x, start_y * 2);
	change_color(4);
	cout << "бс";
#endif
	get<0>(bo[start_x][start_y]) = '@';
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
					if (!is_valid(nx, ny) || get<0>(bo[nx][ny]) != '.') continue;
					if (is_dead_end(bo, nx, ny)) dead_ends.erase(find(all(dead_ends), pair<int, int>(nx, ny)));
				}

#if defined(PRINT_BOARD) && defined(PRINT_GO)
				gotoxy(now[i].X, now[i].Y * 2);
				change_color(7);
				cout << "бс";
#endif
				get<0>(bo[now[i].X][now[i].Y]) = '.';
				++total_cell;
			}
			roll_back.pop_back();
			qpath.pop_back();
		}
		qpath += "URDL"[dir];

		vector<pair<int, int>> walks;
		walks.emplace_back(x, y);
		bool impossible = just_go(bo, walks, x, y, dir);
		roll_back.emplace_back(walks);

		if (impossible) continue;
		if (total_cell == 0) return qpath;

		for (int i = 1; i < 4; i += 2) {
			int nxt_x = x + dx[(dir + i) % 4];
			int nxt_y = y + dy[(dir + i) % 4];
			if (is_valid(nxt_x, nxt_y) && get<0>(bo[nxt_x][nxt_y]) == '.') trace.emplace_back(x, y, (dir + i) % 4, num + 1);
		}
	}

	while (!roll_back.empty()) {
		auto& now = roll_back.back();
		for (int i = sz(now); --i; ) {
			auto [x, y] = now[i - 1];
			for (int j = 0; j < 4; ++j) {
				int nx = x + dx[j];
				int ny = y + dy[j];
				if (!is_valid(nx, ny) || get<0>(bo[nx][ny]) != '.') continue;
				if (is_dead_end(bo, nx, ny)) dead_ends.erase(find(all(dead_ends), pair<int, int>(nx, ny)));
			}

#if defined(PRINT_BOARD) && defined(PRINT_GO)
			gotoxy(now[i].X, now[i].Y * 2);
			change_color(7);
			cout << "бс";
#endif
			get<0>(bo[now[i].X][now[i].Y]) = '.';
			++total_cell;
		}
		roll_back.pop_back();
	}

#if defined(PRINT_BOARD) && defined(PRINT_GO)
	gotoxy(start_x, start_y * 2);
	change_color(7);
	cout << "бс";
#endif
	get<0>(bo[start_x][start_y]) = '.';
	++total_cell;

	if (is_dead_end(bo, start_x, start_y)) dead_ends.emplace_back(start_x, start_y);

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

	vector<vector<tuple<char, int, int>>> bo(bo_x, vector<tuple<char, int, int>>(bo_y));
	for (int idx = 0, i = 0; i < bo_x; ++i) {
		for (int j = 0; j < bo_y; ++j) {
			bo[i][j] = { bo_info[idx++], -1, -1 };
			if (get<0>(bo[i][j]) == '.') ++total_cell;
		}
	}

	for (int i = 0; i < bo_x; ++i) {
		for (int j = 0; j < bo_y; ++j) {
			if (get<0>(bo[i][j]) != '.') continue;
			if (is_dead_end(bo, i, j)) dead_ends.emplace_back(i, j);
		}
	}

#ifdef PRINT_BOARD
	for (auto& i : bo) {
		for (auto [j, a, b] : i) {
			if (j == 'X') change_color(10);
			else if (j == '.') change_color(7);
			std::cout << "бс";
		}
		std::cout << endl;
	}
#endif

	double total_time = 0;
	for (auto& dead_end : dead_ends) {
		auto [start_x, start_y] = dead_end;
		chrono::system_clock::time_point start = chrono::system_clock::now();

#ifndef PRINT_BOARD
		cout << "(" << start_x << ", " << start_y << ") ";
#endif
		string res = brute_force(bo, start_x, start_y);

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
			cout << "\a\nFINISHED!\nIt takes " << total_time << " seconds.\n\n";

			ofstream out("outurl.txt");
			out << "http://www.hacker.org/coil/index.php?x=" << start_y << "&y=" << start_x << "&qpath=" << res;
			out.close();

			return 0;
		}
	}

	for (int i = bo_x; i--; ) {
		for (int j = bo_y; j--; ) {
			if (get<0>(bo[i][j]) == 'X' || find(all(dead_ends), pair<int, int>(i, j)) != dead_ends.end()) continue;

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
				cout << "\a\nFINISHED!\nIt takes " << total_time << " seconds.\n\n";

				ofstream out("outurl.txt");
				out << "http://www.hacker.org/coil/index.php?x=" << j << "&y=" << i << "&qpath=" << res;
				out.close();

				return 0;
			}
		}
	}

	return 0;
}