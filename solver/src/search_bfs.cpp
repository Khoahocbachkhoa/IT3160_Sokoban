#include <algorithm>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../include/classic_search.h"
#include "../include/search.h"

struct ParentInfoBFS {
    State parent;
    char move;
};

SolveResult solveBFS(const Board& board, const State& start_state, long long time_limit_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();

    SolveResult result{false, 0, 0, "", ""};
    std::queue<State> frontier;
    std::unordered_set<State, StateHash> visited;
    std::unordered_map<State, ParentInfoBFS, StateHash> parent_map;

    State start = start_state;
    start.canonical_player = start.player;

    frontier.push(start);
    visited.insert(start);

    while (!frontier.empty()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - start_time
        ).count();

        if (elapsed > time_limit_ms) {
            result.fail_reason = "đạt giới hạn thời gian";
            result.time_ms = elapsed;
            return result;
        }

        State current = frontier.front();
        frontier.pop();
        ++result.nodes_searched;

        if (board.isSolved(current.boxes)) {
            result.solved = true;

            std::string path;
            State cursor = current;
            while (!(cursor == start)) {
                const ParentInfoBFS& info = parent_map.at(cursor);
                path.push_back(info.move);
                cursor = info.parent;
            }
            std::reverse(path.begin(), path.end());
            result.solution = path;
            result.time_ms = elapsed;
            return result;
        }

        for (const ClassicSuccessor& successor : generateClassicSuccessors(board, current)) {
            if (visited.insert(successor.state).second) {
                parent_map[successor.state] = {current, successor.move};
                frontier.push(successor.state);
            }
        }
    }

    result.fail_reason = "No solution found";
    auto end_time = std::chrono::high_resolution_clock::now();
    result.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time
    ).count();
    return result;
}
