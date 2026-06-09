#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <algorithm>

#include "../include/search.h"

struct SearchNode {
    State state; // trạng thái hiện tại
    
    int g;  // cost from start
    int h;  // heuristic estimate to goal
    
    // ! std::string path; Copy gặp vấn đề hiệu năng

    // weighted A*
    int f() const { return g + 2 * h; }

    bool operator>(const SearchNode& other) const {
        if (f() != other.f()) return f() > other.f();
        return h > other.h;
    }
};

// * Lưu cách để đến được trạng thái của search node
struct ParentInfo {
    State parent;
    std::string move;
};

// * lưu node con và cách di chuyển tới node con đó
struct Successor {
    SearchNode node;
    std::string move;
};

// Sinh ra các trạng thái con từ trạng thái hiện tại
// Chỉ tính các trạng thái đẩy thùng 
std::vector<Successor> generateSuccessors(
    const Board& board,
    const SearchNode& current,
    const DeadlockDetector& detector)
{
    std::vector<Successor> successors;
    const auto& boxes = current.state.boxes;

    // Tính toán tất cả ô player có thể tới với các ô thùng cố định
    DistanceMap reachable = calculateDistances(board, current.state.player, boxes);

    int dr[] = {0, -1, 0, 1};
    int dc[] = {-1, 0, 1, 0};

    // Di chuyển không đẩy thùng
    char walk_chars[] = {'l', 'u', 'r', 'd'};
    // Đẩy thùng
    char push_chars[] = {'L', 'U', 'R', 'D'};

    for (int box_index = 0; box_index < (int)boxes.size(); ++box_index) {
        // * Duyệt qua mỗi box
        int box_p = boxes[box_index];
        int br = board.row(box_p);
        int bc = board.col(box_p);

        for (int dir = 0; dir < 4; ++dir) {
            // * Vị trí của người chơi phải đứng trước khi đẩy
            int player_req_r = br - dr[dir];
            int player_req_c = bc - dc[dir];
            // * Vị trí của thùng sau khi bị đẩy
            int box_to_r = br + dr[dir];
            int box_to_c = bc + dc[dir];

            // * Nếu vị trí không hợp lệ -> Không có khả năng xảy ra trường hợp đó
            if (!board.valid(player_req_r, player_req_c) || !board.valid(box_to_r, box_to_c)) {
                continue;
            }

            int player_req_p = board.id(player_req_r, player_req_c);
            int box_to_p = board.id(box_to_r, box_to_c);

            // * Không thể đẩy và tường hoặc vào thùng khác
            if (board.isWall(box_to_p) || current.state.hasBox(box_to_p)) {
                continue;
            }

            // * Không thể đạt tới vị trị để đầy thùng từ vị trí hiện tại
            if (reachable.get(player_req_p) < 0) {
                continue;
            }

            // * Bỏ qua nếu đẩy thùng vào 1 ô có deadlock
            if (detector.isSimpleDeadlock(box_to_p)) {
                continue;
            }

            // * Khởi tạo trạng thái kế tiếp
            SearchNode next = current;
            next.state = current.state;

            // * Di chuyển người chơi tới ô của thùng
            next.state.player = box_p;

            // * Cập nhật vị trí thùng
            auto erase_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_p);
            next.state.boxes.erase(erase_it);
            auto insert_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_to_p);
            next.state.boxes.insert(insert_it, box_to_p);

            // * Nếu gặp deadlock sau khi đẩy thùng
            if (detector.isDeadlockState(board, next.state.boxes)) {
                continue;
            }

            // * Tìm đường đi đến ô trước khi đẩy thùng
            std::vector<int> walk_path = getPath(board, current.state.player, player_req_p, boxes);
            if (walk_path.empty() && current.state.player != player_req_p) {
                continue;
            }

            std::string move_str;

            // * Thêm vào path các đường đi bộ
            for (int move_dir : walk_path) {
                move_str.push_back(walk_chars[move_dir]);
            }

            // * Thêm vào path các đường đi đẩy thùng
            move_str.push_back(push_chars[dir]);

            next.g = current.g + 1; // * Tính chi phí là 1 lần đẩy thùng
            next.h = enhancedHeuristic(board, next.state.boxes);

            //? Chuẩn hóa vị trí người chơi
            DistanceMap can_reach = calculateDistances(board, next.state.player, next.state.boxes);
            int min_cell = 1e9;

            for (int cell = 0; cell < board.getSize(); ++cell) {
                if (can_reach.get(cell) >= 0) {
                    min_cell = cell;
                    break;
                }
            }
            // Vị trí chuẩn hóa là vị trí nhỏ nhất có thể đi tới
            next.state.canonical_player = min_cell;

            Successor s;
            s.node = next;
            s.move = move_str;

            // * Thêm mới 1 node con
            successors.push_back(std::move(s));
        }
    }

    return successors;
}

SolveResult solveSystem(const Board& board, const State& start_state, long long time_limit_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();

    // * lưu cặp trạng thái con, cha để truy vết
    std::unordered_map<State, ParentInfo, StateHash> parent_map;

    // init result
    SolveResult result;

    result.solved = false;
    result.nodes_searched = 0;
    result.solution = "";
    result.fail_reason = "Timeout or goal not reached";

    // Initialize deadlock detector
    DeadlockDetector detector(board);

    // * Nếu trạng thái khởi tạo có deadlock
    if (detector.isDeadlockState(board, start_state.boxes)) {
        result.fail_reason = "Initial state is deadlock";

        auto end_time = std::chrono::high_resolution_clock::now();
        
        result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        return result;
    }

    // Khởi tạo frontier
    std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode>> frontier;
    std::unordered_set<State, StateHash> visited;

    // Initial node
    SearchNode start_node;
    start_node.state = start_state;
    start_node.g = 0;
    start_node.h = enhancedHeuristic(board, start_state.boxes);
    DistanceMap can_reach = calculateDistances(board, start_node.state.player, start_node.state.boxes);
    int min_cell = 1e9;

    for (int cell = 0; cell < board.getSize(); ++cell) {
        if (can_reach.get(cell) >= 0) {
            min_cell = cell;
            break;
        }
    }

    start_node.state.canonical_player = min_cell;

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

        // * Lấy node có f nhỏ nhất
        SearchNode current = frontier.top();
        frontier.pop();
        ++result.nodes_searched;

        // * Nếu đã đạt được lời giải
        if (board.isSolved(current.state.boxes)) {
            result.solved = true;

            // Truy vết lời giải
            std::string path;
            std::vector<std::string> segments;

            State cur = current.state;
            while (!(cur == start_node.state))
            {
                auto it = parent_map.find(cur);
                segments.push_back(it->second.move);
                cur = it->second.parent;
            }

            std::reverse(segments.begin(), segments.end());

            for (const auto &s : segments)
                path += s;

            result.solution = path;

            auto end_time = std::chrono::high_resolution_clock::now();
            result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            return result;
        }

        // Nếu trạng thái đã được thăm thì bỏ qua
        if (visited.count(current.state))
            continue;

        visited.insert(current.state);

        // tạo các trạng thái con
        auto successors = generateSuccessors(board, current, detector);

        for (const auto& s : successors) {
            if (visited.count(s.node.state) == 0) {
                parent_map[s.node.state] = {
                    current.state,
                    s.move
                };

                frontier.push(s.node);
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