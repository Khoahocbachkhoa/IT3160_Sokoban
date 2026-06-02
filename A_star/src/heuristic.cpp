#include "../include/heuristic.h"
#include <climits>
#include <algorithm>

using namespace std;

// Tính khoảng cách ghép cặp tối thiểu giữa thùng và đích (Greedy)
int hungarianLowerBound(const Board& board, const vector<int>& boxes) {
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

// Hàm Heuristic so khớp tối thiểu
// Chưa tính tới khả năng di động của thùng do chưa xét tường
int minMatchingHeuristic(const Board& board, const vector<int>& boxes) {
    return hungarianLowerBound(board, boxes);
}

// Cải tiến: Cộng thêm trọng số về khả năng di động của thùng
int enhancedHeuristic(const Board& board, const vector<int>& boxes) {
    int base = minMatchingHeuristic(board, boxes);
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