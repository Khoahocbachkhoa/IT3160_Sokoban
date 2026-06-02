#include <iostream>
#include <string>

#include "../include/board.h"
#include "../include/search.h"

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Board board;
    State start_state;

    readBoard(board, start_state);

    if (board.getSize() == 0 || start_state.player == -1) {
        cout << "Bản đồ không hợp lệ!" << endl;
        return 1;
    }

    // Đặt giới hạn thời gian
    long long time_limit = 1000000000;

    SolveResult result = solveSystem(board, start_state, time_limit);

    if (result.solved) {
        cout << "Tìm ra lời giải!" << endl;
        cout << "Thời gian chạy    : " << result.time_ms << " ms" << endl;
        cout << "Số lần đẩy thùng  : " << result.pushes <<  endl;
        cout << "Số bước di chuyển : " << result.moves  <<  endl;
        cout << "Số node duyệt     : " << result.nodes_searched << " trạng thái" << endl;
        cout << "Chuỗi di chuyển   : " << result.solution << endl;
    } else {
        cout << "Không tìm thấy lời giải!" << endl;
        cout << "Thời gian chạy : " << result.time_ms << " ms" << endl;
        cout << "Lý do          : " << result.fail_reason << endl;
    }

    return 0;
}