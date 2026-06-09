#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <queue>

#include "../include/board.h"
#include "../include/distance.h"
#include "../include/search.h"
#include "../include/deadlock.h"
#include "../include/heuristic.h"

struct SearchNode {
    State state;
    
    int g;
    int h;

    // int f() const { return g + 2 * h; }
    int f() const { return g + h; }

    bool operator>(const SearchNode& other) const {
        if (f() != other.f()) return f() > other.f();
        return h > other.h;
    }
};

struct ParentInfo {
    State parent;
    std::string move;
};

struct Successor {
    SearchNode node;
    std::string move;
};

// sinh ra các trạng thái con
std::vector<Successor> generateSuccessors(
    const Board& board,
    const SearchNode& current,
    const DeadlockDetector& detector) 
{
    std::vector<Successor> successors;
    const auto& boxes = current.state.boxes;

    int dr[] = {0, -1, 0, 1};
    int dc[] = {-1, 0, 1, 0};
    char walk_chars[] = {'l', 'u', 'r', 'd'};
    char push_chars[] = {'L', 'U', 'R', 'D'};

    DistanceMap reachable = calculateDistances(board, current.state.player, boxes);

    for (int box_index = 0; box_index < (int)boxes.size(); ++box_index) {
        int box_p = boxes[box_index];
        int br = board.row(box_p);
        int bc = board.col(box_p);

        for (int dir = 0; dir < 4; ++dir) {
            // Vị trí người chơi đứng để đẩy thùng 
            int player_req_r = br - dr[dir];
            int player_req_c = bc - dc[dir];
            // Vị trí thùng sau khi đẩy
            int box_to_r = br + dr[dir];
            int box_to_c = bc + dc[dir];

            // Nếu vị trí không hợp lệ
            if (!board.valid(player_req_r, player_req_c) || !board.valid(box_to_r, box_to_c))
                continue;

            int player_req_p = board.id(player_req_r, player_req_c);
            int box_to_p = board.id(box_to_r, box_to_c);

            // Nếu đẩy vào tường hoặc thùng khác
            if (board.isWall(box_to_p) || current.state.hasBox(box_to_p)) {
                continue;
            }

            // Người chơi không thể tới vị trí để đẩy thùng
            if (reachable.get(player_req_p) < 0) {
                continue;
            }

            // Đẩy thùng vào một ô có deadlock
            if (detector.isSimpleDeadlock(box_to_p)) {
                continue;
            }

            SearchNode next = current;
            next.state = current.state;
            next.state.player = box_p;

            // Cập nhật vị trí thùng
            auto erase_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_p);
            next.state.boxes.erase(erase_it);
            auto insert_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_to_p);
            next.state.boxes.insert(insert_it, box_to_p);

            // Nếu gặp freeze deadlock
            if (detector.isFreezeDeadlock(board, next.state.boxes)) {
                continue;
            }

            // Tìm đường cho người chơi di chuyển tới vị trí hiện tại
            std::vector<int> walk_path = getPath(board, current.state.player, player_req_p, boxes);

            std::string move_str;

            // Bước đi bộ
            for (int move_dir : walk_path) {
                move_str.push_back(walk_chars[move_dir]);
            }
            // Bước đẩy thùng
            move_str.push_back(push_chars[dir]);

            next.g = current.g + 1;
            next.h = heuristic(board, next.state.boxes);

            // Chuẩn hóa vị trí nhỏ nhất mà người chơi có thể di chuyển được
            DistanceMap can_reach = calculateDistances(board, next.state.player, next.state.boxes);
            next.state.canonical_player = can_reach.get_min_cell();

            // thêm mới node con
            successors.push_back({next, move_str});
        }
    }

    return successors;
}

SolveResult solveSystem(const Board& board, const State& start_state, long long time_limit_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::unordered_map<State, ParentInfo, StateHash> parent_map;

    SolveResult result;
    DeadlockDetector detector(board);

    result.solved = false;
    result.nodes_searched = 0;
    result.solution = "";

    if (detector.isDeadlockState(board, start_state.boxes)) {
        result.fail_reason = "trạng thái khởi tạo có bế tắc";

        auto end_time = std::chrono::high_resolution_clock::now();
        result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        return result;
    }

    std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode>> frontier;
    std::unordered_set<State, StateHash> visited;

    SearchNode start_node;

    start_node.state = start_state;
    start_node.g = 0;
    start_node.h = heuristic(board, start_state.boxes);

    DistanceMap can_reach = calculateDistances(board, start_node.state.player, start_node.state.boxes);
    start_node.state.canonical_player = can_reach.get_min_cell();

    frontier.push(start_node);

    while (!frontier.empty()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

        if (elapsed > time_limit_ms) {
            result.fail_reason = "đạt giới hạn thời gian";
            result.time_ms = elapsed;
            result.nodes_searched = frontier.size();
            return result;
        }

        SearchNode current = frontier.top();
        frontier.pop();
        ++result.nodes_searched;

        // Tìm thấy lời giải
        if (board.isSolved(current.state.boxes)) {
            result.solved = true;

            std::string path;
            std::vector<std::string> segments;

            State cur = current.state;
            // Truy vết lời giải
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

        if (visited.count(current.state))
            continue;

        visited.insert(current.state);

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