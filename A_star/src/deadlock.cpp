// deadlock.cpp
#include "../include/deadlock.h"
#include <algorithm>
using namespace std;

DeadlockDetector::DeadlockDetector(const Board& board) {
    simple_deadlocks.assign(board.getSize(), false);
    computeSimpleDeadlocks(board);
}

// Khởi tạo cạnh chết và góc chết
void DeadlockDetector::computeSimpleDeadlocks(const Board& board) {
    for (int p = 0; p < board.getSize(); ++p) {
        if (board.isWall(p) || board.isGoal(p)) {
            continue;
        }

        // Kiểm tra góc chết
        if (isCornerDeadlock(board, p)) {
            simple_deadlocks[p] = true;
            continue;
        }

        // Kiểm tra cạnh chết
        if (isEdgeDeadlock(board, p)) {
            simple_deadlocks[p] = true;
        }
    }
}

// Ô p có phải là một góc chết ko (bị kẹt trong góc vuông)
bool DeadlockDetector::isCornerDeadlock(const Board& board, int p) const {
    if (board.isGoal(p)) {
        return false;
    }

    int r = board.row(p);
    int c = board.col(p);

    // Lấy trạng thái tường của 4 ô xung quanh
    bool up_blocked    = !board.valid(r - 1, c) || board.isWall(board.id(r - 1, c));
    bool right_blocked = !board.valid(r, c + 1) || board.isWall(board.id(r, c + 1));
    bool down_blocked  = !board.valid(r + 1, c) || board.isWall(board.id(r + 1, c));
    bool left_blocked  = !board.valid(r, c - 1) || board.isWall(board.id(r, c - 1));

    // Nếu bị chặn bởi góc vuông
    return (up_blocked && right_blocked) ||
           (right_blocked && down_blocked) ||
           (down_blocked && left_blocked) ||
           (left_blocked && up_blocked);
}

// Ô p có nằm trên một cạnh chết ko
bool DeadlockDetector::isEdgeDeadlock(const Board& board, int p) const {
    if (board.isGoal(p)) {
        return false;
    }

    int r = board.row(p);
    int c = board.col(p);

    // 1. Kiểm tra Cạnh Chết Ngang (tường ở trên hoặc dưới)
    bool up_wall   = !board.valid(r - 1, c) || board.isWall(board.id(r - 1, c));
    bool down_wall = !board.valid(r + 1, c) || board.isWall(board.id(r + 1, c));

    if (up_wall || down_wall) {
        bool has_goal_on_edge = false;
        bool has_exit = false;

        auto checkCell = [&](int rr, int cc) {
            int check_p = board.id(rr, cc);
            if (board.isGoal(check_p)) {
                has_goal_on_edge = true;
            }
            bool check_up   = !board.valid(rr - 1, cc) || board.isWall(board.id(rr - 1, cc));
            bool check_down = !board.valid(rr + 1, cc) || board.isWall(board.id(rr + 1, cc));
            if ((up_wall && !check_up) || (down_wall && !check_down)) {
                has_exit = true;
            }
        };

        // Duyệt đoạn liên tục trên cùng một hàng
        for (int check_c = c - 1; board.valid(r, check_c) && !board.isWall(board.id(r, check_c)); --check_c) {
            checkCell(r, check_c);
            if (has_goal_on_edge || has_exit) break;
        }
        for (int check_c = c + 1; !has_goal_on_edge && !has_exit && board.valid(r, check_c) && !board.isWall(board.id(r, check_c)); ++check_c) {
            checkCell(r, check_c);
            if (has_goal_on_edge || has_exit) break;
        }

        if (!has_goal_on_edge && !has_exit) {
            return true;
        }
    }

    // 2. Kiểm tra Cạnh Chết Dọc (tường ở trái hoặc phải)
    bool left_wall  = !board.valid(r, c - 1) || board.isWall(board.id(r, c - 1));
    bool right_wall = !board.valid(r, c + 1) || board.isWall(board.id(r, c + 1));

    if (left_wall || right_wall) {
        bool has_goal_on_edge = false;
        bool has_exit = false;

        auto checkCell = [&](int rr, int cc) {
            int check_p = board.id(rr, cc);
            if (board.isGoal(check_p)) {
                has_goal_on_edge = true;
            }
            bool check_left  = !board.valid(rr, cc - 1) || board.isWall(board.id(rr, cc - 1));
            bool check_right = !board.valid(rr, cc + 1) || board.isWall(board.id(rr, cc + 1));
            if ((left_wall && !check_left) || (right_wall && !check_right)) {
                has_exit = true;
            }
        };

        for (int check_r = r - 1; board.valid(check_r, c) && !board.isWall(board.id(check_r, c)); --check_r) {
            checkCell(check_r, c);
            if (has_goal_on_edge || has_exit) break;
        }
        for (int check_r = r + 1; !has_goal_on_edge && !has_exit && board.valid(check_r, c) && !board.isWall(board.id(check_r, c)); ++check_r) {
            checkCell(check_r, c);
            if (has_goal_on_edge || has_exit) break;
        }

        if (!has_goal_on_edge && !has_exit) {
            return true;
        }
    }

    return false;
}

bool DeadlockDetector::isSimpleDeadlock(int p) const {
    if (p < 0 || p >= (int)simple_deadlocks.size()) {
        return false;
    }
    return simple_deadlocks[p];
}

// Kiểm tra trạng thái kẹt sao cho không thể đẩy được thùng
bool DeadlockDetector::isFreezeDeadlock(const Board& board, const vector<int>& boxes) const {
    // ô (r,c) có phải là tường hoặc thùng ko
    auto isWallOrBox = [&](int r, int c) -> bool {
        if (!board.valid(r, c)) return true;
        int p = board.id(r, c);
        if (board.isWall(p)) return true;
        return binary_search(boxes.begin(), boxes.end(), p);
    };

    for (int box_p : boxes) {
        if (board.isGoal(box_p)) {
            continue;
        }

        int r = board.row(box_p);
        int c = board.col(box_p);

        // Duyệt 4 góc vuông 2x2 xung quanh chiếc thùng hiện tại
        for (int dy = 0; dy <= 1; ++dy) {
            for (int dx = 0; dx <= 1; ++dx) {

                int r1 = r, c1 = c;
                int r2 = r, c2 = c + (dx == 0 ? -1 : 1);
                int r3 = r + (dy == 0 ? -1 : 1), c3 = c;
                int r4 = r + (dy == 0 ? -1 : 1), c4 = c + (dx == 0 ? -1 : 1);

                // Nếu cả 4 ô là thùng hoặc tường
                if (isWallOrBox(r2, c2) && isWallOrBox(r3, c3) && isWallOrBox(r4, c4)) {
                    // Nếu có 1 ô chứa thùng và không nằm trên đích -> deadlock (không thể đẩy được thùng)
                    auto isBoxAndNotGoal = [&](int r_c, int c_c) -> bool {
                        if (!board.valid(r_c, c_c)) return false;
                        int p = board.id(r_c, c_c);
                        return !board.isGoal(p) && binary_search(boxes.begin(), boxes.end(), p);
                    };

                    if (isBoxAndNotGoal(r1, c1) || isBoxAndNotGoal(r2, c2) || 
                        isBoxAndNotGoal(r3, c3) || isBoxAndNotGoal(r4, c4)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool DeadlockDetector::isDeadlockState(const Board& board, const vector<int>& boxes) const {
    for (int box_p : boxes) {
        if (isSimpleDeadlock(box_p)) {
            return true;
        }
    }

    return isFreezeDeadlock(board, boxes);
}