#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "deadlock.h"
#include "distance.h"
#include "heuristic.h"

#include <string>
#include <vector>

struct SolveResult {
    bool solved;
    long long time_ms; // Thời gian chạy
    int pushes;        // Số lần đẩy thùng
    int moves;         // Số bước di chuyển
    long long nodes_searched;   // Số node đã duyệt
    std::string solution;       // Các bước để đi tới lời giải
    std::string fail_reason;    // Lý do thất bại
};

SolveResult solveSystem(const Board& board, const State& start_state, long long time_limit_ms);

#endif