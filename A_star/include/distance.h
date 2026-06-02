#ifndef DISTANCE_H
#define DISTANCE_H

#include "board.h"
#include <vector>

const int DIR_LEFT  = 0;
const int DIR_UP    = 1;
const int DIR_RIGHT = 2;
const int DIR_DOWN  = 3;

// Lưu khoảng cách di chuyển giữa các ô trong map
// Các ô đích là các ô có thể di chuyển được tới mà không đẩy thùng
struct DistanceMap {
    // * distances[p] = số bước tới ô có id là p
    // * distances[p] = -1: ô p ko tới được
    std::vector<int> distances; 
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

// Tính toán khoảng cách từ ô start tới tất cả các ô có thể đi được
DistanceMap calculateDistances(const Board& board, int start_p, const std::vector<int>& boxes);

// Kiểm tra xem người chơi có thể đi bộ từ ô 'from_p' đến ô 'to_p' không
bool canReach(const Board& board, int from_p, int to_p, const std::vector<int>& boxes);

// * Tìm đường đi chi tiết từ 'from_p' đến 'to_p'
// trả về vector các bước dạng 0, 1, 2, 3.. (DIR_LEFT, DIR_UP, ..)
// Nếu không tìm thấy lộ trình, trả về vector rỗng.
std::vector<int> getPath(const Board& board, int from_p, int to_p, const std::vector<int>& boxes);
std::vector<int> getPath(DistanceMap &dmap, int to_p);

#endif