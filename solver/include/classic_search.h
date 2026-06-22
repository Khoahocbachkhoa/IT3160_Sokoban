#ifndef CLASSIC_SEARCH_H
#define CLASSIC_SEARCH_H

#include <vector>

#include "board.h"

struct ClassicSuccessor {
    State state;
    char move;
};

// Sinh trạng thái theo đúng một bước di chuyển của người chơi.
// Không chuẩn hóa vị trí người chơi, không deadlock pruning, không heuristic.
std::vector<ClassicSuccessor> generateClassicSuccessors(
    const Board& board,
    const State& current
);

#endif
