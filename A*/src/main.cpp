#include <bits/stdc++.h>
using namespace std;

// '#' : Tường đá
// ' ' : Ô trống
// '$' : Thùng
// '.' : Đích
// '@' : Người chơi
// '*' : Thùng trên ô đích 
// '+' : Người chơi trên đích

// Input:
// ########
// #  .   #
// # $  @ #
// #      #
// ########

// Output:
// Chuỗi các hành động để tới được trạng thái đích

// Method:
// Manhattan distance
// Hungarian heuristic
// deadlock detection
// pattern database

constexpr int MAXROW = 30;
constexpr int MAXCOL = 30;
constexpr int MAXN = MAXROW * MAXCOL;

int row, col;
bitset<MAXN> walkable; // walkable[i] = 1 : nếu ô có vị trí tương đối i có thể đi được
bitset<MAXN> goals; // goals[i] = 1: nếu ô có vị trí tương đối i là một đích 
bitset<MAXN> walls; // walls[i] = 1 : nếu ô có vị trí tương đối i là tường

struct State {
    /* Vị trí tương đối của người chơi so với ô 0 */
    int player; 
    /* Vị trí hiện tại của các thùng */
    bitset<MAXN> boxes;

    bool operator==(State const& other) const {
        return player == other.player && boxes == other.boxes;
    }
};

void input(State &initialState) {
    cin >> row >> col;
    cin.ignore();

    for (int i = 0; i < row; ++i) {
        string line;
        getline(cin, line);

        for (int j = 0; j < col; ++j) {
            char ch = line[j];
            
            if (ch != '#') 
                walkable.set(i * col + j);
            else {
                walls.set(i * col + j);
                continue;
            }

            if (ch == '$') {
                initialState.boxes.set(i * col + j);
            } else if (ch == '.') {
                goals.set(i * col + j);
            } else if (ch == '@') {
                initialState.player = i * col + j;
            } else if (ch == '*') {
                initialState.boxes.set(i * col + j);
                goals.set(i * col + j);
            } else if (ch == '+') {
                initialState.player = i * col + j;
                goals.set(i * col + j);
            }
        }
    }
}

int main() {
    
}
