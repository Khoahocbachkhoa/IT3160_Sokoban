#include "../include/distance.h"
#include <queue>
#include <algorithm>

using namespace std;

// 4 hướng di chuyển nhất quán toàn dự án: Trái, Trên, Phải, Dưới
static const int dr[] = {0, -1, 0, 1};
static const int dc[] = {-1, 0, 1, 0};

// Calculate distances from a position to all reachable cells
DistanceMap calculateDistances(const Board& board, int start_p, const vector<int>& boxes) {
    DistanceMap dist_map(board.getCols(), board.getRows());
    queue<pair<int, int>> q; // pair<vị_trí_index_1D, khoảng_cách_hiện_tại>

    q.push({start_p, 0});
    dist_map.set(start_p, 0);

    while (!q.empty()) {
        auto [curr_p, dist] = q.front();
        q.pop();

        int r = board.row(curr_p);
        int c = board.col(curr_p);

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (!board.valid(nr, nc)) continue;

            int next_p = board.id(nr, nc);

            if (board.isWall(next_p)) continue;
            // Kiểm tra nhị phân xem ô tiếp theo có bị thùng cản không
            if (binary_search(boxes.begin(), boxes.end(), next_p)) continue; 
            if (dist_map.get(next_p) != -1) continue; // Ô này đã được tính khoảng cách ngắn nhất trước đó

            dist_map.set(next_p, dist + 1);
            q.push({next_p, dist + 1});
        }
    }

    return dist_map;
}

// Check if player can reach a position
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

            if (next_p == to_p) return true; // Chạm được tới đích đến
            if (board.isWall(next_p)) continue;
            if (binary_search(boxes.begin(), boxes.end(), next_p)) continue; // Bị chặn bởi thùng
            if (visited[next_p]) continue;

            visited[next_p] = true;
            q.push(next_p);
        }
    }

    return false;
}

// Get the path from one position to another (returns direction indices)
vector<int> getPath(const Board& board, int from_p, int to_p, const vector<int>& boxes) {
    vector<int> path;
    if (from_p == to_p) return path;

    vector<bool> visited(board.getSize(), false);
    // parent[next_p] = {ô_bước_trước_đó, mã_hướng_di_chuyển_i}
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
            if (binary_search(boxes.begin(), boxes.end(), next_p)) continue; // Vướng thùng
            if (visited[next_p]) continue;

            visited[next_p] = true;
            parent[next_p] = {curr_p, i};
            q.push(next_p);
        }
    }

    // Reconstruct path (Truy vết ngược lại lộ trình để chuyển thành hướng đi chuẩn)
    if (found) {
        int curr = to_p;
        while (parent[curr].first != -1) {
            path.push_back(parent[curr].second);
            curr = parent[curr].first;
        }
        reverse(path.begin(), path.end()); // Đảo mảng để có lộ trình xuôi từ xuất phát -> đích
    }

    return path;
}