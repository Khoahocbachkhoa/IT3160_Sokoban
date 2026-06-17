#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <stack>

#include "../include/board.h"
#include "../include/distance.h"
#include "../include/search.h"
#include "../include/deadlock.h"
#include "../include/heuristic.h"

struct SearchNodeDFS {
    State state;
    int g;
    int h;
};

struct ParentInfoDFS {
    State parent;
    std::string move;
};

struct SuccessorDFS {
    SearchNodeDFS node;
    std::string move;
};

std::vector<SuccessorDFS> generateSuccessorsDFS(
    const Board& board,
    const SearchNodeDFS& current,
    const DeadlockDetector& detector) 
{
    std::vector<SuccessorDFS> successors;
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
            int player_req_r = br - dr[dir];
            int player_req_c = bc - dc[dir];
            int box_to_r = br + dr[dir];
            int box_to_c = bc + dc[dir];

            if (!board.valid(player_req_r, player_req_c) || !board.valid(box_to_r, box_to_c))
                continue;

            int player_req_p = board.id(player_req_r, player_req_c);
            int box_to_p = board.id(box_to_r, box_to_c);

            if (board.isWall(box_to_p) || current.state.hasBox(box_to_p)) continue;
            if (reachable.get(player_req_p) < 0) continue;
            if (detector.isSimpleDeadlock(box_to_p)) continue;

            SearchNodeDFS next = current;
            next.state = current.state;
            next.state.player = box_p;

            auto erase_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_p);
            next.state.boxes.erase(erase_it);
            auto insert_it = std::lower_bound(next.state.boxes.begin(), next.state.boxes.end(), box_to_p);
            next.state.boxes.insert(insert_it, box_to_p);

            if (detector.isFreezeDeadlock(board, next.state.boxes)) continue;

            std::vector<int> walk_path = getPath(board, current.state.player, player_req_p, boxes);

            std::string move_str;
            for (int move_dir : walk_path) move_str.push_back(walk_chars[move_dir]);
            move_str.push_back(push_chars[dir]);

            next.g = current.g + 1;
            next.h = heuristic(board, next.state.boxes);

            DistanceMap can_reach = calculateDistances(board, next.state.player, next.state.boxes);
            next.state.canonical_player = can_reach.get_min_cell();

            successors.push_back({next, move_str});
        }
    }

    return successors;
}

SolveResult solveDFS(const Board& board, const State& start_state, long long time_limit_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::unordered_map<State, ParentInfoDFS, StateHash> parent_map;

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

    std::stack<SearchNodeDFS> frontier;
    std::unordered_set<State, StateHash> visited;

    SearchNodeDFS start_node;
    start_node.state = start_state;
    start_node.g = 0;
    start_node.h = heuristic(board, start_state.boxes);

    DistanceMap can_reach = calculateDistances(board, start_node.state.player, start_node.state.boxes);
    start_node.state.canonical_player = can_reach.get_min_cell();

    frontier.push(start_node);
    visited.insert(start_node.state);

    while (!frontier.empty()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

        if (elapsed > time_limit_ms) {
            result.fail_reason = "đạt giới hạn thời gian";
            result.time_ms = elapsed;
            result.nodes_searched = frontier.size();
            return result;
        }

        SearchNodeDFS current = frontier.top(); frontier.pop();
        ++result.nodes_searched;

        if (board.isSolved(current.state.boxes)) {
            result.solved = true;

            std::string path;
            std::vector<std::string> segments;

            State cur = current.state;
            while (!(cur == start_node.state)) {
                auto it = parent_map.find(cur);
                segments.push_back(it->second.move);
                cur = it->second.parent;
            }
            std::reverse(segments.begin(), segments.end());
            for (const auto &s : segments) path += s;

            result.solution = path;

            auto end_time = std::chrono::high_resolution_clock::now();
            result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            return result;
        }

        auto successors = generateSuccessorsDFS(board, current, detector);

        for (const auto& s : successors) {
            if (visited.count(s.node.state) == 0) {
                parent_map[s.node.state] = { current.state, s.move };
                visited.insert(s.node.state);
                frontier.push(s.node);
            }
        }
    }

    result.fail_reason = "No solution found";
    auto end_time = std::chrono::high_resolution_clock::now();
    result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    result.nodes_searched = visited.size();
    return result;
}
