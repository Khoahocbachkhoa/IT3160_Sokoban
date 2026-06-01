#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "board.h"
#include <vector>

// Tính khoảng cách manhataan giữa 2 ô
inline int manhattanDistance(const Board& board, int p1, int p2) {
    int r1 = board.row(p1);
    int c1 = board.col(p1);
    int r2 = board.row(p2);
    int c2 = board.col(p2);
    return std::abs(r1 - r2) + std::abs(c1 - c2);
}

// Tính toán khoảng cách manhattan giữa các thùng của trạng thái hiên tại tới ô đích
int hungarianLowerBound(const Board& board, const std::vector<int>& boxes);

// Hàm Heuristic so khớp tối thiểu
int minMatchingHeuristic(const Board& board, const std::vector<int>& boxes);

// Hàm Heuristic nâng cao : tính thêm penalty dựa vào khả năng di động của thùng
int enhancedHeuristic(const Board& board, const std::vector<int>& boxes);

#endif