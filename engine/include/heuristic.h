#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "board.h"
#include "deadlock.h"
#include <vector>

inline int manhattanDistance(const Board& board, int p1, int p2) {
    int r1 = board.row(p1);
    int c1 = board.col(p1);
    int r2 = board.row(p2);
    int c2 = board.col(p2);
    return std::abs(r1 - r2) + std::abs(c1 - c2);
}

// Heuristic 1 : Ghép cặp mỗi box - goal và tính tổng khoảng cách manhattan

// Sử dụng thuật toán tham lam để ghép cặp
int heuristicManhattanMatchingGreedy(const Board& board, const std::vector<int>& boxes);

// Sử dụng thuật toán ghép cặp cực đại Hungarian
int heuristicManhattanMatchingHungarian(const Board& board, const std::vector<int>& boxes);

// Heuristic 2 : Cải tiến từ heuristic 1, thêm điểm phạt dựa trên khả năng di động của thùng
int enhancedHeuristicManhattanMatching(const Board& board, const std::vector<int>& boxes);

// Heuristic 3 : Ghép cặp mỗi box-goal và tính tổng dựa trên số lần đẩy thùng thay vì k/c manhattan

// ghép cặp dùng tham lam
int heuristicPushDistanceGreedy(const Board& board, const std::vector<int>& boxes);

// ghép cặp sử dụng thuật toán hungarian
int heuristicPushDistanceHungarian(const Board& board, const std::vector<int>& boxes);

int heuristic(const Board& board, const std::vector<int>& boxes);

#endif