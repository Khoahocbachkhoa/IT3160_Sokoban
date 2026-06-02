#ifndef DISTANCE_H
#define DISTANCE_H

#include "board.h"
#include <vector>

// 0: Trái, 1: Trên, 2: Phải, 3: Dưới 
const int DIR_LEFT  = 0;
const int DIR_UP    = 1;
const int DIR_RIGHT = 2;
const int DIR_DOWN  = 3;

// Cấu trúc DistanceMap tương đương lưu kết quả tính toán khoảng cách
struct DistanceMap {
    std::vector<int> distances; // distances[p] = số bước, nếu bằng -1 nghĩa là không thể tới (None)
    int width;
    int height;

    DistanceMap(int w, int h) : width(w), height(h) {
        distances.assign(w * h, -1);
    }

    // Lấy khoảng cách tại ô p
    int get(int p) const {
        if (p < 0 || p >= (int)distances.size()) return -1;
        return distances[p];
    }

    // Cập nhật khoảng cách tại ô p
    void set(int p, int dist) {
        if (p >= 0 && p < (int)distances.size()) {
            distances[p] = dist;
        }
    }
};

// Tính toán khoảng cách từ ô start đến tất cả các ô người chơi có thể chạm tới
DistanceMap calculateDistances(const Board& board, int start_p, const std::vector<int>& boxes);

// Kiểm tra xem người chơi có thể đi bộ từ ô 'from_p' đến ô 'to_p' không
bool canReach(const Board& board, int from_p, int to_p, const std::vector<int>& boxes);

// Tìm đường đi chi tiết từ 'from_p' đến 'to_p' (Trả về danh sách các mã hướng 0, 1, 2, 3)
// Nếu không tìm thấy lộ trình, trả về vector rỗng.
std::vector<int> getPath(const Board& board, int from_p, int to_p, const std::vector<int>& boxes);

#endif