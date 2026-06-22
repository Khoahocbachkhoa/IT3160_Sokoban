#include <algorithm>

#include "../include/classic_search.h"

std::vector<ClassicSuccessor> generateClassicSuccessors(
    const Board& board,
    const State& current)
{
    std::vector<ClassicSuccessor> successors;

    const int dr[] = {0, -1, 0, 1};
    const int dc[] = {-1, 0, 1, 0};
    const char walk_chars[] = {'l', 'u', 'r', 'd'};
    const char push_chars[] = {'L', 'U', 'R', 'D'};

    int player_r = board.row(current.player);
    int player_c = board.col(current.player);

    for (int dir = 0; dir < 4; ++dir) {
        int next_r = player_r + dr[dir];
        int next_c = player_c + dc[dir];

        if (!board.valid(next_r, next_c)) {
            continue;
        }

        int next_p = board.id(next_r, next_c);
        if (board.isWall(next_p)) {
            continue;
        }

        State next = current;
        char move = walk_chars[dir];

        if (current.hasBox(next_p)) {
            int box_to_r = next_r + dr[dir];
            int box_to_c = next_c + dc[dir];

            if (!board.valid(box_to_r, box_to_c)) {
                continue;
            }

            int box_to_p = board.id(box_to_r, box_to_c);
            if (board.isWall(box_to_p) || current.hasBox(box_to_p)) {
                continue;
            }

            auto box_it = std::lower_bound(next.boxes.begin(), next.boxes.end(), next_p);
            next.boxes.erase(box_it);
            next.boxes.insert(
                std::lower_bound(next.boxes.begin(), next.boxes.end(), box_to_p),
                box_to_p
            );
            move = push_chars[dir];
        }

        next.player = next_p;
        next.canonical_player = next.player;
        successors.push_back({next, move});
    }

    return successors;
}
