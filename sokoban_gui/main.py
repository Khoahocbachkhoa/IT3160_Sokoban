import sys

import pygame

from config import FPS, MIN_WINDOW_HEIGHT, SIDEBAR_WIDTH, TILE_SIZE
from src.assets import AssetManager
from src.button import Button
from src.game import Game
from src.renderer import Renderer
from src.solver_client import ALGORITHMS, SolverClient


def create_buttons(fonts):
    return [
        Button(0, 0, 1, 1, "SOLVE", fonts["bold_22"], "solve"),
        Button(0, 0, 1, 1, "STEP", fonts["bold_22"], "step"),
        Button(0, 0, 1, 1, "PAUSE", fonts["bold_22"], "pause"),
        Button(0, 0, 1, 1, "RETRY", fonts["bold_22"], "retry"),
        Button(0, 0, 1, 1, "NEXT", fonts["bold_22"], "next"),
    ]


def create_algorithm_buttons(fonts):
    buttons = []
    for algorithm in ALGORITHMS:
        buttons.append(Button(0, 0, 1, 1, algorithm.upper(), fonts["bold_22"], f"algorithm:{algorithm}"))
    buttons.append(Button(0, 0, 1, 1, "CANCEL", fonts["bold_22"], "algorithm:cancel"))
    return buttons


def handle_button(button, game, buttons):
    if button.action == "solve":
        return "open_algorithm_menu"
    elif button.action == "step":
        if game.step_solution():
            print(f"STEP {game.solution_index}/{len(game.solution)}")
        else:
            print("STEP clicked: no solution step available")
    elif button.action == "pause":
        game.toggle_pause()
        button.text = "RESUME" if game.paused else "PAUSE"
    elif button.action == "retry":
        game.retry()
        reset_pause_label(buttons)
    elif button.action == "next":
        game.next_level()
        reset_pause_label(buttons)
    return None


def handle_algorithm_button(button, game, solver_client):
    if button.action == "algorithm:cancel":
        return "close_algorithm_menu"

    algorithm = button.action.split(":", 1)[1]
    print(f"SOLVE clicked: {algorithm}")
    game.solver_message = f"Solving with {algorithm}..."

    result = solver_client.solve(game.level.grid, algorithm)
    print(result.stdout, end="")
    if result.stderr:
        print(result.stderr, end="")

    if result.solved:
        game.set_solution(result.solution)
        game.solver_message = f"{algorithm}: solution ready ({len(result.solution)} steps)"
        print(f"Solution ({algorithm}): {result.solution}")
    else:
        game.set_solution("")
        game.solver_message = f"{algorithm}: {result.message}"
        print(f"No solution ({algorithm}): {result.message}")

    return "close_algorithm_menu"


def reset_pause_label(buttons):
    for button in buttons:
        if button.action == "pause":
            button.text = "PAUSE"


def main():
    pygame.init()
    pygame.display.set_caption("Sokoban")

    game = Game()
    board_width = game.board_cols * TILE_SIZE
    board_height = game.board_rows * TILE_SIZE
    window_size = (board_width + SIDEBAR_WIDTH + 96, max(board_height + 96, MIN_WINDOW_HEIGHT))
    screen = pygame.display.set_mode(window_size, pygame.RESIZABLE)
    clock = pygame.time.Clock()

    assets = AssetManager()
    buttons = create_buttons(assets.fonts)
    algorithm_buttons = create_algorithm_buttons(assets.fonts)
    solver_client = SolverClient()
    renderer = Renderer(assets)
    algorithm_menu_open = False

    running = True
    while running:
        mouse_pos = pygame.mouse.get_pos()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            elif event.type == pygame.VIDEORESIZE:
                width = max(event.w, 700)
                height = max(event.h, 480)
                screen = pygame.display.set_mode((width, height), pygame.RESIZABLE)

            elif event.type == pygame.KEYDOWN:
                if event.key in (pygame.K_UP, pygame.K_w):
                    if game.move_player(-1, 0):
                        game.set_solution("")
                elif event.key in (pygame.K_DOWN, pygame.K_s):
                    if game.move_player(1, 0):
                        game.set_solution("")
                elif event.key in (pygame.K_LEFT, pygame.K_a):
                    if game.move_player(0, -1):
                        game.set_solution("")
                elif event.key in (pygame.K_RIGHT, pygame.K_d):
                    if game.move_player(0, 1):
                        game.set_solution("")
                elif event.key == pygame.K_r:
                    game.retry()
                    reset_pause_label(buttons)
                elif event.key == pygame.K_p:
                    game.toggle_pause()
                    for button in buttons:
                        if button.action == "pause":
                            button.text = "RESUME" if game.paused else "PAUSE"

            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                if algorithm_menu_open:
                    for button in algorithm_buttons:
                        if button.is_clicked(mouse_pos):
                            action = handle_algorithm_button(button, game, solver_client)
                            if action == "close_algorithm_menu":
                                algorithm_menu_open = False
                            break
                else:
                    for button in buttons:
                        if button.is_clicked(mouse_pos):
                            action = handle_button(button, game, buttons)
                            if action == "open_algorithm_menu":
                                algorithm_menu_open = True
                            break

        renderer.draw(screen, game, buttons, mouse_pos, algorithm_menu_open, algorithm_buttons)
        pygame.display.flip()
        clock.tick(FPS)

    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
