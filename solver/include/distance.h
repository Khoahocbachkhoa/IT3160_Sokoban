#ifndef DISTANCE_H
#define DISTANCE_H

#include "board.h"
#include <vector>

const int DIR_LEFT  = 0;
const int DIR_UP    = 1;
const int DIR_RIGHT = 2;
const int DIR_DOWN  = 3;

struct DistanceMap {
    // distances[p] = số bước tới ô có id là p
    // distances[p] = -1: ô p ko tới được
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

    // Ô nhỏ nhất nằm trong phạm vi
    int get_min_cell() const {
        int min_cell = 1e9;

        for (int cell = 0; cell < (int)distances.size(); ++cell) {
            if (this->get(cell) >= 0) {
                min_cell = cell;
                break;
            }
        }

        return min_cell;
    }
};

DistanceMap calculateDistances(const Board& board, int start_p, const std::vector<int>& boxes);

bool canReach(const Board& board, int from_p, int to_p, const std::vector<int>& boxes);

// Tìm đường đi chi tiết từ 'from_p' đến 'to_p'
// Trả về vector các bước dạng 0, 1, 2, 3.. (DIR_LEFT, DIR_UP, ..) hoặc vector rỗng nếu không tìm thấy
std::vector<int> getPath(const Board& board, int from_p, int to_p, const std::vector<int>& boxes);

#endif