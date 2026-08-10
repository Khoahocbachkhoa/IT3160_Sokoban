#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "board.h"
#include <vector>

class DeadlockDetector {
private:
    // is_deadlocks[p] = true nếu đẩy thùng vào ô p sẽ tạo deadlock state
    std::vector<bool> is_deadlocks;

    void computeSimpleDeadlocks(const Board& board);

    void computeDeadSquares(const Board& board);

    bool isCornerDeadlock(const Board& board, int p) const;

    bool isEdgeDeadlock(const Board& board, int p) const;

public:
    // Init
    DeadlockDetector(const Board& board);

    bool isSimpleDeadlock(int p) const;

    bool isFreezeDeadlock(const Board& board, const std::vector<int>& boxes) const;

    bool isDeadlockState(const Board& board, const std::vector<int>& boxes) const;
};

#endif