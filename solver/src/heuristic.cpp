#include "../include/heuristic.h"
#include "../include/deadlock.h"
#include <climits>
#include <algorithm>

using namespace std;

// Bổ trợ: thuật toán hungarian ghép cặp trên đồ thị 2 phía
int hungarian(const vector<vector<int>>& a) {
    int n = (int)a.size();

    vector<int> u(n + 1), v(n + 1);
    vector<int> p(n + 1), way(n + 1);

    for (int i = 1; i <= n; ++i) {
        p[0] = i;

        int j0 = 0;
        vector<int> minv(n + 1, INT_MAX);
        vector<bool> used(n + 1, false);

        do {
            used[j0] = true;

            int i0 = p[j0];
            int delta = INT_MAX;
            int j1 = 0;

            for (int j = 1; j <= n; ++j) {
                if (used[j]) continue;

                int cur = a[i0 - 1][j - 1] - u[i0] - v[j];

                if (cur < minv[j]) {
                    minv[j] = cur;
                    way[j] = j0;
                }

                if (minv[j] < delta) {
                    delta = minv[j];
                    j1 = j;
                }
            }

            for (int j = 0; j <= n; ++j) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }

            j0 = j1;

        } while (p[j0] != 0);

        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    int cost = 0;

    vector<int> assignment(n);

    for (int j = 1; j <= n; ++j) {
        assignment[p[j] - 1] = j - 1;
        cost += a[p[j] - 1][j - 1];
    }

    return cost;
}

int heuristicManhattanMatchingGreedy(const Board& board, const vector<int>& boxes) {
    size_t n = boxes.size();
    if (n == 0) {
        return 0;
    }

    const vector<int>& goals = board.goals;

    vector<bool> used_goals(goals.size(), false);
    int total_cost = 0;

    for (int box_pos : boxes) {
        int min_dist = INT_MAX;
        size_t best_goal = 0;

        // Tìm đích khả dụng gần nhất với thùng
        for (size_t i = 0; i < goals.size(); ++i) {
            if (used_goals[i]) {
                continue;
            }
            
            int dist = manhattanDistance(board, box_pos, goals[i]);
            if (dist < min_dist) {
                min_dist = dist;
                best_goal = i;
            }
        }

        // Đánh dấu đích đã được dùng
        if (min_dist < INT_MAX) {
            used_goals[best_goal] = true;
            total_cost += min_dist;
        }
    }

    return total_cost;
}

int heuristicManhattanMatchingHungarian(const Board& board, const std::vector<int>& boxes) {
    int n = (int)boxes.size();

    const std::vector<int>& goals = board.goals;

    std::vector<std::vector<int>> cost(n, std::vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cost[i][j] = manhattanDistance(board, boxes[i], goals[j]);
        }
    }

    return hungarian(cost);
}

int enhancedHeuristicManhattanMatching(const Board& board, const vector<int>& boxes) {
    int base = heuristicManhattanMatchingGreedy(board, boxes);
    // * int base = heuristicManhattanMatchingHungarian(board, boxes);
    int penalty = 0;

    int dr[] = {0, 1, 0, -1};
    int dc[] = {-1, 0, 1, 0};

    for (int box_pos : boxes) {
        // Nếu thùng đã nằm đúng ô đích thì không tính điểm phạt nữa
        if (board.isGoal(box_pos)) {
            continue;
        }

        int blocked = 0;
        int r = board.row(box_pos);
        int c = board.col(box_pos);

        for (int i = 0; i < 4; ++i) {
            int next_r = r + dr[i];
            int next_c = c + dc[i];
            
            // Nếu cạnh 1 ô không hợp lệ
            if (!board.valid(next_r, next_c)) {
                blocked++;
                continue;
            }

            int next_p = board.id(next_r, next_c);

            // Nếu ô tiếp là tường
            if (board.isWall(next_p)) {
                blocked++;
            }
        }

        // Cộng thêm điểm phạt nếu thùng bị kẹt
        if (blocked >= 3) {
            penalty += 15;
        } else if (blocked == 2) {
            penalty += 2;
        }
    }

    return base + penalty;
}

int heuristicPushDistanceGreedy(const Board& board, const vector<int>& boxes) {
    constexpr int PUSH_INF = 1000000000;
    constexpr int BIG = 1000000;

    int n = (int)board.goals.size();

    vector<bool> used_goal(n, false);

    int h = 0;

    for (int box : boxes) {
        int best_goal = -1;
        int best_cost = BIG;

        for (int g = 0; g < n; ++g) {
            if (used_goal[g])
                continue;

            int d = board.push_dist[g][box];

            if (d >= PUSH_INF)
                d = BIG;

            if (d < best_cost) {
                best_cost = d;
                best_goal = g;
            }
        }

        if (best_goal == -1)
            return BIG;

        used_goal[best_goal] = true;
        h += best_cost;
    }

    return h;
}

int heuristicPushDistanceHungarian(const Board& board, const std::vector<int>& boxes) {
    constexpr int PUSH_INF = 1000000000;
    constexpr int BIG = 1000000;

    int n = (int)boxes.size();

    if (n == 0) {
        return 0;
    }

    std::vector<std::vector<int>> cost(n, std::vector<int>(n));

    for (int b = 0; b < n; ++b) {
        bool reachable = false;
        for (int g = 0; g < n; ++g) {
            int d = board.push_dist[g][boxes[b]];

            if (d >= PUSH_INF) {
                d = BIG;
            } else {
                reachable = true;
            }

            cost[b][g] = d;
        }
        if (!reachable)
            return BIG;
    }

    return hungarian(cost);
}

int heuristic(const Board& board, const std::vector<int>& boxes) {
    return heuristicPushDistanceHungarian(board, boxes);
}
