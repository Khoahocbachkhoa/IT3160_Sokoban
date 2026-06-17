#include <queue>
#include <algorithm>

#include "../include/distance.h"

using namespace std;

// Trái, trên, phải, dưới
static const int dr[] = {0, -1, 0, 1};
static const int dc[] = {-1, 0, 1, 0};

DistanceMap calculateDistances(const Board& board, int start_p, const vector<int>& boxes) {
    DistanceMap dist_map(board.getCols(), board.getRows());

    queue<pair<int, int>> q; // {p, khoảng cách tới p}

    q.push({start_p, 0});
    dist_map.set(start_p, 0);

    // BFS tìm khoảng cách
    while (!q.empty()) {
        auto [curr_p, dist] = q.front();
        q.pop();

        int r = board.row(curr_p);
        int c = board.col(curr_p);

        // Thử đi theo các hướng
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            // Gặp ô không hợp lệ 
            if (!board.valid(nr, nc)) continue;

            // Gặp tường
            int next_p = board.id(nr, nc);
            if (board.isWall(next_p)) continue;

            // Gặp thùng
            if (binary_search(boxes.begin(), boxes.end(), next_p)) continue; 
            
            // Gặp ô đã được đi trước đó
            if (dist_map.get(next_p) != -1) continue;

            // Đánh dấu ô đã được thăm
            dist_map.set(next_p, dist + 1);
            q.push({next_p, dist + 1});
        }
    }

    return dist_map;
}

// Kiểm tra có thể đi được từ ô from_p tới ô to_p không
bool canReach(const Board& board, int from_p, int to_p, const vector<int>& boxes) {
    if (from_p == to_p) return true;

    vector<bool> visited(board.getSize(), false);
    queue<int> q;

    q.push(from_p);
    visited[from_p] = true;

    while (!q.empty()) {
        int curr_p = q.front();
        q.pop();

        int r = board.row(curr_p);
        int c = board.col(curr_p);

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (!board.valid(nr, nc)) continue;

            int next_p = board.id(nr, nc);
            if (next_p == to_p) return true; // * Nếu tới được đích
            
            if (board.isWall(next_p)) continue;
            if (binary_search(boxes.begin(), boxes.end(), next_p)) continue;
            if (visited[next_p]) continue;

            visited[next_p] = true;
            q.push(next_p);
        }
    }

    return false;
}

// * trả về chuỗi di chuyển từ ô from_p tới to_p
vector<int> getPath(const Board& board, int from_p, int to_p, const vector<int>& boxes) {
    vector<int> path;
    if (from_p == to_p) return path;

    vector<bool> visited(board.getSize(), false);
    // parent[next_p] = {id ô trước, bước di chuyển (UDLR))}
    vector<pair<int, int>> parent(board.getSize(), {-1, -1}); 
    queue<int> q;

    q.push(from_p);
    visited[from_p] = true;
    bool found = false;

    while (!q.empty()) {
        int curr_p = q.front();
        q.pop();

        if (curr_p == to_p) {
            found = true;
            break;
        }

        int r = board.row(curr_p);
        int c = board.col(curr_p);

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (!board.valid(nr, nc)) continue;

            int next_p = board.id(nr, nc);
            if (board.isWall(next_p)) continue;
            if (binary_search(boxes.begin(), boxes.end(), next_p)) continue;
            
            if (visited[next_p]) continue;

            visited[next_p] = true;
            parent[next_p] = {curr_p, i};
            q.push(next_p);
        }
    }

    // truy vết đường đi nếu tìm thấy
    if (found) {
        int curr = to_p;
        while (parent[curr].first != -1) {
            path.push_back(parent[curr].second);
            curr = parent[curr].first;
        }
        reverse(path.begin(), path.end());
    }

    return path;
}