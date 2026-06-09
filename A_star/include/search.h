#ifndef SEARCH_H
#define SEARCH_H

#include <string>

#include "board.h"

struct SolveResult {
    bool solved;
    long long time_ms;
    long long nodes_searched;
    std::string solution;
    std::string fail_reason;
};

SolveResult solveSystem(const Board& board, const State& start_state, long long time_limit_ms);

#endif