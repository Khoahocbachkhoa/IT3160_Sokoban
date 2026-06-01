#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "board.h"
#include <vector>

/* Note: Thuật toán chỉ phát hiện được các deadlock đơn giản */
/* Các deadlock phức tạp như gặp hốc cụt ... chưa được xét tới */
class DeadlockDetector {
private:
    // simple_deaddlocks[p] = true nếu p là cạnh chết hoặc góc chết tĩnh
    std::vector<bool> simple_deadlocks;

    // Tính toán các góc chết tĩnh dựa vào cấu trúc tường của bản đồ
    void computeSimpleDeadlocks(const Board& board);

    // Ô p có phải là một góc chết ko (bị kẹt trong góc vuông)
    bool isCornerDeadlock(const Board& board, int p) const;

    // Ô p có phải cạnh chết không (Kẹt trong tường và trên cạnh không có đích)
    bool isEdgeDeadlock(const Board& board, int p) const;

public:
    // Khởi tạo detector
    DeadlockDetector(const Board& board);

    // Kiểm tra một ô có phải deadlock đơn giản ko
    bool isSimpleDeadlock(int p) const;

    // Kiểm tra các dạng kẹt sao cho không thể đẩy thùng (ví dụ 3 thùng, 1 đá tạo thành 2x2)
    bool isFreezeDeadlock(const Board& board, const std::vector<int>& boxes) const;

    // Kiểm tra trạng thái thùng hiện tại có bị dính deadlock không
    bool isDeadlockState(const Board& board, const std::vector<int>& boxes) const;
};

#endif