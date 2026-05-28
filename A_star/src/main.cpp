#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdint>
using namespace std;

struct Position {
    int8_t x;
    int8_t y;

    Position() : x(0), y(0) {}
    
    Position(int8_t x, int8_t y) : x(x), y(y) {}

    uint32_t manhattan_distance(const Position &o) const {
        return abs(x - o.x) + abs(y - o.y); 
    }
};

enum Cell {
    Wall,
    Floor,
    Goal
};

struct Board {
    size_t width;
    size_t height;

    vector<vector<Cell>> cells;

    Position player;
    vector<Position> boxes;
    vector<Position> goals;

    Board(size_t w = 0, size_t h = 0) : 
        width(w),
        height(h),
        cells(h, std::vector<Cell>(w)) {}
};



int main() {

}