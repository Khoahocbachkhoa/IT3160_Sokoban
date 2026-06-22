#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <queue>

#include "../include/board.h"
#include "../include/distance.h"
#include "../include/search.h"
#include "../include/deadlock.h"
#include "../include/heuristic.h"

static const int HEURISTIC_INF = 1000000;
static const int HEURISTIC_WEIGHT = 1;
static const char WALK_CHARS[] = {'l', 'u', 'r', 'd'};
static const char PUSH_CHARS[] = {'L', 'U', 'R', 'D'};

struct SearchNode {
    State state;
    
    int g;
    int h;

    int f() const { return g + HEURISTIC_WEIGHT * h; }

    bool operator>(const SearchNode& other) const {
        if (f() != other.f()) return f() > other.f();
        return h > other.h;
    }
};

struct ParentInfo {
    State parent;
    int player_required;
    char push;
};

struct Successor {
    SearchNode node;
    int player_required;
    char push;
};

struct BoxesHash {
    size_t operator()(const std::vector<int>& boxes) const {
        size_t hash_value = 0;
        for (int box : boxes) {
            hash_value ^= std::hash<int>{}(box)
                + 0x9e3779b9
                + (hash_value << 6)
                + (hash_value >> 2);
        }
        return hash_value;
    }
};

static int playerApproachLowerBound(const Board& board, const State& state) {
    if (board.isSolved(state.boxes)) {
        return 0;
    }

    int best = HEURISTIC_INF;
    for (int box : state.boxes) {
        int distance = manhattanDistance(board, state.player, box);
        best = std::min(best, std::max(0, distance - 1));
    }
    return best == HEURISTIC_INF ? 0 : best;
}

// sinh ra các trạng thái con
std::vector<Successor> generateSuccessors(
    const Board& board,
    const SearchNode& current,
    const DeadlockDetector& detector,
    std::unordered_map<std::vector<int>, int, BoxesHash>& heuristic_cache)
{
    std::vector<Successor> successors;
    const auto& boxes = current.state.boxes;

    int dr[] = {0, -1, 0, 1};
    int dc[] = {-1, 0, 1, 0};
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

            // Chi phí A* là tổng số thao tác: các bước đi ngắn nhất để tới
            // phía sau thùng, cộng một thao tác đẩy.
            next.g = current.g + reachable.get(player_req_p) + 1;
            int box_h;
            auto cached_h = heuristic_cache.find(next.state.boxes);
            if (cached_h == heuristic_cache.end()) {
                box_h = heuristic(board, next.state.boxes);
                heuristic_cache.emplace(next.state.boxes, box_h);
            } else {
                box_h = cached_h->second;
            }
            if (box_h >= HEURISTIC_INF) {
                continue;
            }
            next.h = box_h + playerApproachLowerBound(board, next.state);

            // Khi tối ưu tổng step, vị trí thật của người chơi ảnh hưởng chi
            // phí đi tới cú đẩy tiếp theo nên không được gộp cả vùng reachable.
            next.state.canonical_player = next.state.player;

            // thêm mới node con
            successors.push_back({next, player_req_p, PUSH_CHARS[dir]});
        }
    }

    return successors;
}

SolveResult solveAStar(const Board& board, const State& start_state, long long time_limit_ms) {
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
    std::unordered_map<State, int, StateHash> best_g;
    std::unordered_map<std::vector<int>, int, BoxesHash> heuristic_cache;

    SearchNode start_node;

    start_node.state = start_state;
    start_node.g = 0;
    int start_box_h = heuristic(board, start_state.boxes);
    start_node.h = start_box_h + playerApproachLowerBound(board, start_node.state);
    heuristic_cache.emplace(start_node.state.boxes, start_box_h);

    if (start_box_h >= HEURISTIC_INF) {
        result.fail_reason = "không tồn tại phép ghép thùng-đích khả thi";
        auto end_time = std::chrono::high_resolution_clock::now();
        result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time
        ).count();
        return result;
    }

    start_node.state.canonical_player = start_node.state.player;

    frontier.push(start_node);
    best_g[start_node.state] = 0;

    while (!frontier.empty()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

        if (elapsed > time_limit_ms) {
            result.fail_reason = "đạt giới hạn thời gian";
            result.time_ms = elapsed;
            return result;
        }

        SearchNode current = frontier.top();
        frontier.pop();

        auto current_best = best_g.find(current.state);
        if (current_best == best_g.end() || current.g != current_best->second) {
            continue;
        }

        ++result.nodes_searched;

        // Tìm thấy lời giải
        if (board.isSolved(current.state.boxes)) {
            result.solved = true;

            std::vector<std::string> segments;

            State cur = current.state;
            // Truy vết lời giải
            while (!(cur == start_node.state))
            {
                const ParentInfo& info = parent_map.at(cur);
                std::vector<int> walk_path = getPath(
                    board,
                    info.parent.player,
                    info.player_required,
                    info.parent.boxes
                );

                std::string segment;
                for (int move_dir : walk_path) {
                    segment.push_back(WALK_CHARS[move_dir]);
                }
                segment.push_back(info.push);
                segments.push_back(segment);
                cur = info.parent;
            }

            std::reverse(segments.begin(), segments.end());

            for (const std::string& segment : segments) {
                result.solution += segment;
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            return result;
        }

        auto successors = generateSuccessors(board, current, detector, heuristic_cache);

        for (const auto& s : successors) {
            auto known = best_g.find(s.node.state);
            if (known == best_g.end() || s.node.g < known->second) {
                best_g[s.node.state] = s.node.g;
                parent_map[s.node.state] = {
                    current.state,
                    s.player_required,
                    s.push
                };

                frontier.push(s.node);
            }
        }
    }

    // Không tìm được lời giải
    result.fail_reason = "No solution found";
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    return result;
}

// keep old name for compatibility
SolveResult solveSystem(const Board& board, const State& start_state, long long time_limit_ms) {
    return solveAStar(board, start_state, time_limit_ms);
}
