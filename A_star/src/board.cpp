#include <iostream>
#include "../include/board.h"
using namespace std;

// reset trạng thái của bảng
void Board::clear() {
    rows = cols = 0;
    is_wall.clear();
    is_goal.clear();
    goals.clear();
}

// ô chữ đã được giải chưa
bool Board::isSolved(const vector<int>& boxes) const {
    for (int box : boxes) {
        if (!is_goal[box]) return false;
    }
    return true;
}

// dùng cho set, map
bool State::operator<(const State& other) const {
    if (player != other.player) 
        return player < other.player;
    return boxes < other.boxes;
}

bool State::operator==(const State& other) const {
    return player == other.player && boxes == other.boxes;
}

// Hàm băm cho state
size_t StateHash::operator()(const State& s) const {
    size_t h = hash<int>{}(s.player);
    
    for (int box : s.boxes) {
        h ^= hash<int>{}(box) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
}

// đọc ô chữ từ bàn phím
void readBoard(Board& board, State& start) {
    board.clear();
    start = State();

    cin >> board.rows >> board.cols;
    cin.ignore();

    int n = board.rows * board.cols;

    board.is_wall.assign(n, false);
    board.is_goal.assign(n, false);

    for (int r = 0; r < board.rows; r++) {
        string line;
        getline(cin, line);

        // padding _
        if ((int)line.size() < board.cols)
            line.resize(board.cols, ' ');

        for (int c = 0; c < board.cols; c++) {
            char ch = line[c];
            int p = board.id(r, c);

            switch (ch) {
                case '#': /// Tường
                    board.is_wall[p] = true;
                    break;

                case '.': // Đích
                    board.is_goal[p] = true;
                    board.goals.push_back(p);
                    break;

                case '$': // Thùng
                    start.boxes.push_back(p);
                    break;

                case '@': // Người chơi
                    start.player = p;
                    break;

                case '*': // Thùng trên đích
                    board.is_goal[p] = true;
                    board.goals.push_back(p);
                    start.boxes.push_back(p);
                    break;

                case '+': // Người chơi trên đích
                    board.is_goal[p] = true;
                    board.goals.push_back(p);
                    start.player = p;
                    break;

                default:
                    break; // ô trống
            }
        }
    }

    sort(start.boxes.begin(), start.boxes.end());
}