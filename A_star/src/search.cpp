#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <algorithm>

#include "../include/search.h"

struct SearchNode {
    State state;
    int g;  // cost from start
    int h;  // heuristic estimate to goal
    int pushes; // số lần đẩy thùng
    int moves;  // số bước di chuyển
    std::string path;

    int f() const { return g + h; }

    bool operator>(const SearchNode& other) const {
        if (f() != other.f()) return f() > other.f();
        return h > other.h;
    }
};

// Sinh ra các trạng thái con từ trạng thái hiện tại
std::vector<SearchNode> generateSuccessors(
    const Board& board, 
    const SearchNode& current, 
    const DeadlockDetector& detector) 
{
    std::vector<SearchNode> successors;

    // Lấy vị trí hiện tại của người chơi
    int player = current.state.player;
    int pr = board.row(player);
    int pc = board.col(player);

    // 4 hướng tương ứng : up, down, left, right
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    char moves_char[] = {'U', 'D', 'L', 'R'};

    for (int i = 0; i < 4; i++) {
        // vị trí mới của player
        int nr = pr + dr[i];
        int nc = pc + dc[i];

        // Không được đi vào tường hoặc ô không hợp lệ
        if (!board.valid(nr, nc) || board.isWall(board.id(nr, nc))) {
            continue;
        }

        int next_player = board.id(nr, nc);
        
        // tạo node mới 
        SearchNode next = current;
        ++next.moves;
        ++next.g;
        next.path += moves_char[i];

        // Nếu tại vị trí mới có thùng
        if (current.state.hasBox(next_player)) {
            // Cần phải đầy thùng
            int box_r = nr + dr[i];
            int box_c = nc + dc[i];

            // Không thể đẩy thùng vào tường hoặc ô không hợp lệ
            if (!board.valid(box_r, box_c) || board.isWall(board.id(box_r, box_c))) {
                continue;
            }

            int box_next = board.id(box_r, box_c);

            // Không thể đẩy thùng nếu vướng 1 cái thùng khác
            if (current.state.hasBox(box_next)) {
                continue;
            }

            // Nếu đẩy thùng vào 1 vị trí có bế tắc
            if (detector.isSimpleDeadlock(box_next)) {
                continue;
            }

            // Nếu đẩy được thùng
            next.state.player = next_player;
            next.state.boxes = current.state.boxes;

            // Thay đổi vị trí thùng
            auto erase_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), next_player);
            next.state.boxes.erase(erase_it);

            auto insert_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_next);
            next.state.boxes.insert(insert_it, box_next);

            next.pushes++;
        } else {
            // Không cần đẩy thùng
            next.state.player = next_player;
        }

        //next.h = minMatchingHeuristic(board, next.state.boxes);
        next.h = enhancedHeuristic(board, next.state.boxes);
        successors.push_back(next);
    }

    return successors;
}

SolveResult solveSystem(const Board& board, const State& start_state, long long time_limit_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();

    // init result
    SolveResult result;
    // hasher for state
    StateHash hasher;

    result.solved = false;
    result.pushes = 0;
    result.moves = 0;
    result.nodes_searched = 0;
    result.solution = "";
    result.fail_reason = "Timeout or goal not reached";

    // Initialize deadlock detector
    DeadlockDetector detector(board);

    // Nếu trạng thái khởi tạo có deadlock
    if (detector.isDeadlockState(board, start_state.boxes)) {
        result.fail_reason = "Initial state is deadlock";

        auto end_time = std::chrono::high_resolution_clock::now();
        
        result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        return result;
    }

    // Khởi tạo frontier
    std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode>> frontier;
    std::unordered_set<size_t> visited;

    // Initial node
    SearchNode start_node;
    start_node.state = start_state;
    start_node.g = 0;
    start_node.h = enhancedHeuristic(board, start_state.boxes);
    start_node.pushes = 0;
    start_node.moves = 0;
    start_node.path = "";

    frontier.push(start_node);

    while (!frontier.empty()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

        if (elapsed > time_limit_ms) {
            result.fail_reason = "Timeout exceeded";
            result.time_ms = elapsed;
            result.nodes_searched = frontier.size();
            return result;
        }

        // Lấy ra node có f() nhỏ nhất
        SearchNode current = frontier.top();
        frontier.pop();
        result.nodes_searched++;

        // Nếu đã đạt được lời giải
        if (board.isSolved(current.state.boxes)) {
            result.solved = true;
            result.pushes = current.pushes;
            result.moves = current.moves;
            result.solution = current.path;
            
            auto end_time = std::chrono::high_resolution_clock::now();
            
            result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            return result;
        }

        // tạo hàm băm cho trạng thái đã thăm
        size_t hash_value = hasher(current.state);

        // Nếu trạng thái đã được thăm
        if (visited.count(hash_value)) {
            continue;
        } else {
            visited.insert(hash_value);
        }

        // tạo các trạng thái con
        auto successors = generateSuccessors(board, current, detector);

        for (const SearchNode& successor : successors) {
            size_t hash_value_succ = hasher(successor.state);

            // Nếu chưa được thăm thì thêm vào frontier
            if (!visited.count(hash_value_succ)) {
                frontier.push(successor);
            }
        }
    }

    // Không tìm được lời giải
    result.fail_reason = "No solution found";
    
    auto end_time = std::chrono::high_resolution_clock::now();
    
    result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    result.nodes_searched = visited.size();

    return result;
}