#include <algorithm>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <vector>

#include "../include/search.h"

struct SearchNodeUCS {
    State state;
    int g;
    long long order;
};

struct ParentInfoUCS {
    State parent;
    char move;
};

struct CompareUCS {
    bool operator()(const SearchNodeUCS& a, const SearchNodeUCS& b) const {
        if (a.g != b.g) {
            return a.g > b.g;
        }
        return a.order > b.order;
    }
};

SolveResult solveUCS(const Board& board, const State& start_state, long long time_limit_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();

    SolveResult result{false, 0, 0, "", ""};
    std::priority_queue<SearchNodeUCS, std::vector<SearchNodeUCS>, CompareUCS> frontier;
    std::unordered_map<State, int, StateHash> best_g;
    std::unordered_map<State, ParentInfoUCS, StateHash> parent_map;
    long long insertion_order = 0;

    State start = start_state;
    start.canonical_player = start.player;

    frontier.push({start, 0, insertion_order++});
    best_g[start] = 0;

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

        SearchNodeUCS current = frontier.top();
        frontier.pop();

        auto known_current = best_g.find(current.state);
        if (known_current == best_g.end() || known_current->second != current.g) {
            continue;
        }

        ++result.nodes_searched;

        if (board.isSolved(current.state.boxes)) {
            result.solved = true;

            std::string path;
            State cursor = current.state;
            while (!(cursor == start)) {
                const ParentInfoUCS& info = parent_map.at(cursor);
                path.push_back(info.move);
                cursor = info.parent;
            }
            std::reverse(path.begin(), path.end());
            result.solution = path;
            result.time_ms = elapsed;
            return result;
        }

        for (const Successor& successor : generateSuccessors(board, current.state)) {
            int next_g = current.g + 1;
            auto known = best_g.find(successor.state);

            if (known == best_g.end() || next_g < known->second) {
                best_g[successor.state] = next_g;
                parent_map[successor.state] = {current.state, successor.move};
                frontier.push({successor.state, next_g, insertion_order++});
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
