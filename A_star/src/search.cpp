// search.cpp
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

// Sinh ra các trạng thái con từ trạng thái hiện tại bằng cách chỉ tạo các trạng thái đẩy thùng
std::vector<SearchNode> generateSuccessors(
    const Board& board,
    const SearchNode& current,
    const DeadlockDetector& detector)
{
    std::vector<SearchNode> successors;
    const auto& boxes = current.state.boxes;

    // Tính toán tất cả ô player có thể tới với các ô thùng cố định
    DistanceMap reachable = calculateDistances(board, current.state.player, boxes);

    // 4 hướng tương ứng : left, up, right, down
    int dr[] = {0, -1, 0, 1};
    int dc[] = {-1, 0, 1, 0};
    char direction_chars[] = {'L', 'U', 'R', 'D'};

    for (int box_index = 0; box_index < (int)boxes.size(); ++box_index) {
        int box_p = boxes[box_index];
        int br = board.row(box_p);
        int bc = board.col(box_p);

        for (int dir = 0; dir < 4; ++dir) {
            int player_req_r = br - dr[dir];
            int player_req_c = bc - dc[dir];
            int box_to_r = br + dr[dir];
            int box_to_c = bc + dc[dir];

            if (!board.valid(player_req_r, player_req_c) || !board.valid(box_to_r, box_to_c)) {
                continue;
            }

            int player_req_p = board.id(player_req_r, player_req_c);
            int box_to_p = board.id(box_to_r, box_to_c);

            if (board.isWall(box_to_p) || current.state.hasBox(box_to_p)) {
                continue;
            }

            if (reachable.get(player_req_p) < 0) {
                continue;
            }

            // Nếu đẩy thùng vào vị trí deadlock, bỏ qua ngay
            if (detector.isSimpleDeadlock(box_to_p)) {
                continue;
            }

            // Xây dựng trạng thái kế tiếp khi đẩy thùng
            SearchNode next = current;
            next.state = current.state;

            // Di chuyển người chơi tới vị trí sau khi đẩy (ô ban đầu của thùng)
            next.state.player = box_p;
            next.state.boxes = current.state.boxes;

            // Cập nhật vị trí thùng
            auto erase_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_p);
            next.state.boxes.erase(erase_it);
            auto insert_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_to_p);
            next.state.boxes.insert(insert_it, box_to_p);

            // Kiểm tra trạng thái deadlock sau khi đẩy
            if (detector.isDeadlockState(board, next.state.boxes)) {
                continue;
            }

            // Tính đường đi người chơi từ vị trí hiện tại đến vị trí đẩy
            std::vector<int> walk_path = getPath(board, current.state.player, player_req_p, boxes);
            if (walk_path.empty() && current.state.player != player_req_p) {
                continue;
            }

            for (int move_dir : walk_path) {
                next.path.push_back(direction_chars[move_dir]);
            }
            next.path.push_back(direction_chars[dir]);

            next.moves += (int)walk_path.size() + 1;
            next.g = current.g + 1; // cost by pushes, not total moves
            next.pushes = current.pushes + 1;
            next.h = enhancedHeuristic(board, next.state.boxes);
            successors.push_back(next);
        }
    }

    return successors;
}

SolveResult solveSystem(const Board& board, const State& start_state, long long time_limit_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();

    // init result
    SolveResult result;

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

    // Khởi tạo frontier và bản đồ chi phí tốt nhất
    std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode>> frontier;
    std::unordered_map<State, int, StateHash> best_cost;

    // Initial node
    SearchNode start_node;
    start_node.state = start_state;
    start_node.g = 0;
    start_node.h = enhancedHeuristic(board, start_state.boxes);
    start_node.pushes = 0;
    start_node.moves = 0;
    start_node.path = "";

    frontier.push(start_node);
    best_cost[start_state] = 0;

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

        // Nếu trạng thái đã được thăm với chi phí tốt hơn, bỏ qua
        auto it = best_cost.find(current.state);
        if (it != best_cost.end() && current.g > it->second) {
            continue;
        }

        // tạo các trạng thái con
        auto successors = generateSuccessors(board, current, detector);

        for (const SearchNode& successor : successors) {
            auto best_it = best_cost.find(successor.state);
            if (best_it == best_cost.end() || successor.g < best_it->second) {
                best_cost[successor.state] = successor.g;
                frontier.push(successor);
            }
        }
    }

    // Không tìm được lời giải
    result.fail_reason = "No solution found";
    
    auto end_time = std::chrono::high_resolution_clock::now();
    
    result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    result.nodes_searched = best_cost.size();

    return result;
}