#include <iostream>
#include <queue>

#include "../include/board.h"
#include "../include/distance.h"

using namespace std;

void Board::clear() {
    rows = cols = 0;
    is_wall.clear();
    is_goal.clear();
    goals.clear();
}

bool Board::isSolved(const vector<int>& boxes) const {
    for (int box : boxes) {
        if (!is_goal[box]) return false;
    }
    return true;
}

bool State::operator<(const State& other) const {
    if (canonical_player != other.canonical_player) 
        return canonical_player < other.canonical_player;
    return boxes < other.boxes;
}

bool State::operator==(const State& other) const {
    return canonical_player == other.canonical_player && boxes == other.boxes;
}

size_t StateHash::operator()(const State& s) const {
    size_t h = hash<int>{}(s.canonical_player);
    
    for (int box : s.boxes) {
        h ^= hash<int>{}(box) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
}

void readBoard(Board& board, State& start) {
    board.clear();
    start = State();

    cin >> board.rows >> board.cols;
    std::string header_line;
    std::getline(cin, header_line);

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

void Board::computePushDistance() {
    const int INF = 1e9;

    push_dist.assign(
        goals.size(),
        vector<int>(getSize(), INF)
    );

    const int dr[] = {-1,1,0,0};
    const int dc[] = {0,0,-1,1};

    for (int g = 0; g < (int)goals.size(); ++g) {
        int goal = goals[g];

        queue<int> q;

        push_dist[g][goal] = 0;
        q.push(goal);

        int r, c;
        while (!q.empty()) {
            int cur = q.front();
            q.pop();

            r = row(cur);
            c = col(cur);

            for (int i = 0; i < 4; ++i) {
                int pre_r = r - dr[i];
                int pre_c = c - dc[i];

                int player_r = pre_r - dr[i];
                int player_c = pre_c - dc[i];

                if (!valid(pre_r, pre_c))
                    continue;

                if (!valid(player_r, player_c))
                    continue;

                int pre_box = id(pre_r, pre_c);
                int player_pos = id(player_r, player_c);

                if (isWall(pre_box))
                    continue;
                if (isWall(player_pos))
                    continue;

                if (push_dist[g][pre_box] == INF) {
                    push_dist[g][pre_box] = push_dist[g][cur] + 1;
                    q.push(pre_box);
                }
            }
        }
    }
}