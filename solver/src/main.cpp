#include <iostream>
#include <string>

#include "../include/board.h"
#include "../include/search.h"

using namespace std;

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string algo = "astar";
    if (argc >= 2) algo = argv[1];

    Board board;
    State start_state;

    readBoard(board, start_state);

    board.computePushDistance();

    if (board.getSize() == 0 || start_state.player == -1) {
        cout << "Invalid map!" << endl;
        return 1;
    }

    long long time_limit = 120000;

    SolveResult result;

    if (algo == "astar") {
        result = solveAStar(board, start_state, time_limit);
    } else if (algo == "bfs") {
        result = solveBFS(board, start_state, time_limit);
    } else if (algo == "dfs") {
        result = solveDFS(board, start_state, time_limit);
    } else if (algo == "ucs") {
        result = solveUCS(board, start_state, time_limit);
    } else {
        cout << "Unknown algorithm: " << algo << "\n";
        cout << "Supported: astar, bfs, dfs, ucs" << endl;
        return 1;
    }

    if (result.solved) {
        cout << "Tìm ra lời giải!" << endl;
        cout << "Thời gian chạy    : " << result.time_ms << " ms" << endl;
        cout << "Số node duyệt     : " << result.nodes_searched << " trạng thái" << endl;
        cout << "Chuỗi di chuyển   : " << result.solution << endl;
    } else {
        cout << "Không tìm thấy lời giải!" << endl;
        cout << "Thời gian chạy : " << result.time_ms << " ms" << endl;
        cout << "Số node duyệt  : " << result.nodes_searched << " trạng thái" << endl;
        cout << "Lý do          : " << result.fail_reason << endl;
    }

    return 0;
}
