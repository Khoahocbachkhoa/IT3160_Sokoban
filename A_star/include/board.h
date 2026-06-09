#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <algorithm>

// Lưu cấu trúc tĩnh của bảng
class Board {
public:
    int rows = 0;
    int cols = 0;

    std::vector<bool> is_wall;  // is_wall[p] = true nếu ô p là tường
    std::vector<bool> is_goal;  // is_goal[p] = true nếu ô p là đích
    
    std::vector<int> goals;     // Danh sách các ô là đích

    // ánh xạ (r,c) -> index mảng 1 chiều
    inline int id(int r, int c) const { 
        return r * cols + c; 
    }
    // index -> row
    inline int row(int p) const { 
        return p / cols; 
    }
    // index -> col
    inline int col(int p) const { 
        return p % cols; 
    }
    // check (r,c)
    inline bool valid(int r, int c) const { 
        return r >= 0 && c >= 0 && r < rows && c < cols; 
    }
    // check p
    inline bool valid(int p) const { 
        return p >= 0 && p < rows * cols; 
    }
    // check if p is wall
    inline bool isWall(int p) const { 
        return is_wall[p]; 
    }
    // check if p is goal
    inline bool isGoal(int p) const { 
        return is_goal[p]; 
    }
    
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getSize() const { return rows * cols; }

    // reset trạng thái của bảng
    void clear();

    // ô chữ đã được giải chưa
    bool isSolved(const std::vector<int>& boxes) const;
};

// Lưu trạn thái động (vị trí người chơi và các thùng)
struct State {
    // Vị trí người chơi
    int player = -1;
    // Ô nhỏ nhất mà người chơi có thể đi đến từ vị trí hiện tại
    // * Dùng để chuẩn hóa khi thêm vào visited
    int canonical_player = -1;
    // Vị trí các thùng sắp xếp theo thứ tự tăng dần 
    std::vector<int> boxes;

    // kiểm tra tại p có thùng nào chưa
    bool hasBox(int p) const {
        return std::binary_search(boxes.begin(), boxes.end(), p);
    }

    // dùng cho set, map
    bool operator<(const State& other) const;

    bool operator==(const State& other) const;
};

// Hàm băm của state
struct StateHash {
    size_t operator()(const State& s) const;
};

// Hàm đọc bảng dữ liệu từ input đầu vào
void readBoard(Board& board, State& start);

#endif