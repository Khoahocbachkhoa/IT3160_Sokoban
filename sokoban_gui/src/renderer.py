import pygame

from config import (
    BOARD_PADDING,
    COLOR_BG,
    COLOR_BOARD_BG,
    COLOR_BOARD_BORDER,
    COLOR_MUTED,
    COLOR_PANEL_LINE,
    COLOR_SIDEBAR,
    COLOR_TEXT,
    COLOR_WIN_OVERLAY,
    SIDEBAR_WIDTH,
    TILE_SIZE,
)


class Renderer:
    def __init__(self, assets):
        self.assets = assets

    def draw(self, screen, game, buttons, mouse_pos, show_algorithm_menu=False, algorithm_buttons=None):
        layout = self._layout(screen, game)
        self._layout_buttons(buttons, layout)
        if algorithm_buttons:
            self._layout_algorithm_buttons(algorithm_buttons, layout)

        screen.fill(COLOR_BG)
        self._draw_board_panel(screen, layout)
        self._draw_board(screen, game, layout)
        self._draw_sidebar(screen, game, buttons, mouse_pos, layout)

        if game.win:
            self._draw_win_overlay(screen, layout)

        if show_algorithm_menu and algorithm_buttons:
            self._draw_algorithm_menu(screen, algorithm_buttons, mouse_pos, layout)

    def _draw_board(self, screen, game, layout):
        for row_index, row in enumerate(game.level.grid):
            for col_index, cell in enumerate(row):
                x = layout["board_rect"].x + col_index * layout["tile_size"]
                y = layout["board_rect"].y + row_index * layout["tile_size"]
                self._draw_cell(screen, cell, x, y, layout["tile_size"])

    def _draw_board_panel(self, screen, layout):
        panel_rect = layout["board_rect"].inflate(18, 18)
        pygame.draw.rect(screen, COLOR_BOARD_BG, panel_rect, border_radius=10)
        pygame.draw.rect(screen, COLOR_BOARD_BORDER, panel_rect, width=2, border_radius=10)

    def _draw_cell(self, screen, cell, x, y, tile_size):
        screen.blit(self.assets.image("ground", tile_size), (x, y))

        if cell == "#":
            screen.blit(self.assets.image("wall", tile_size), (x, y))
        elif cell == ".":
            screen.blit(self.assets.image("target", tile_size), (x, y))
        elif cell == "$":
            screen.blit(self.assets.image("box", tile_size), (x, y))
        elif cell == "@":
            screen.blit(self.assets.image("player", tile_size), (x, y))
        elif cell == "*":
            screen.blit(self.assets.image("target", tile_size), (x, y))
            screen.blit(self.assets.image("box", tile_size), (x, y))
        elif cell == "+":
            screen.blit(self.assets.image("target", tile_size), (x, y))
            screen.blit(self.assets.image("player", tile_size), (x, y))

    def _draw_sidebar(self, screen, game, buttons, mouse_pos, layout):
        sidebar_rect = layout["sidebar_rect"]
        pygame.draw.rect(screen, COLOR_SIDEBAR, sidebar_rect)
        pygame.draw.line(screen, COLOR_PANEL_LINE, (sidebar_rect.x, 0), (sidebar_rect.x, screen.get_height()), 2)

        fonts = self.assets.fonts
        self._draw_centered_text(screen, "SOKOBAN", fonts["bold_42"], COLOR_TEXT, sidebar_rect.centerx, 34)
        self._draw_centered_text(screen, "AI PUZZLE", fonts["regular_18"], COLOR_MUTED, sidebar_rect.centerx, 82)

        info_x = sidebar_rect.x + 42
        self._draw_info_row(screen, "Level", f"{game.level_number:02d}", info_x, 126)
        self._draw_info_row(screen, "Moves", str(game.move_count), info_x, 162)
        self._draw_info_row(screen, "Status", game.status_text(), info_x, 198)

        for button in buttons:
            button.draw(screen, mouse_pos)

        if game.solver_message:
            message_y = max(button.rect.bottom for button in buttons) + 24
            if message_y + 48 <= screen.get_height():
                self._draw_wrapped_text(screen, game.solver_message, fonts["regular_18"], COLOR_MUTED, sidebar_rect.x + 34, message_y, sidebar_rect.width - 68)

        last_button_bottom = max(button.rect.bottom for button in buttons)
        help_y = last_button_bottom + 46
        if help_y + 54 <= screen.get_height():
            self._draw_centered_text(screen, "WASD / ARROWS", fonts["regular_18"], COLOR_MUTED, sidebar_rect.centerx, help_y)
            self._draw_centered_text(screen, "R: RETRY    P: PAUSE", fonts["regular_18"], COLOR_MUTED, sidebar_rect.centerx, help_y + 28)

    def _draw_win_overlay(self, screen, layout):
        board_rect = layout["board_rect"]
        overlay = pygame.Surface((board_rect.width, board_rect.height), pygame.SRCALPHA)
        overlay.fill(COLOR_WIN_OVERLAY)
        screen.blit(overlay, board_rect.topleft)

        text = self.assets.fonts["bold_42"].render("YOU WIN", True, COLOR_TEXT)
        rect = text.get_rect(center=board_rect.center)
        screen.blit(text, rect)

    def _draw_text(self, screen, text, font, color, x, y):
        surface = font.render(text, True, color)
        screen.blit(surface, (x, y))

    def _draw_wrapped_text(self, screen, text, font, color, x, y, max_width):
        words = text.split()
        lines = []
        current = ""
        for word in words:
            test = word if not current else f"{current} {word}"
            if font.size(test)[0] <= max_width:
                current = test
            else:
                if current:
                    lines.append(current)
                current = word
        if current:
            lines.append(current)

        for index, line in enumerate(lines[:2]):
            self._draw_text(screen, line, font, color, x, y + index * 24)

    def _draw_centered_text(self, screen, text, font, color, center_x, y):
        surface = font.render(text, True, color)
        rect = surface.get_rect(midtop=(center_x, y))
        screen.blit(surface, rect)

    def _draw_info_row(self, screen, label, value, x, y):
        fonts = self.assets.fonts
        self._draw_text(screen, label.upper(), fonts["regular_18"], COLOR_MUTED, x, y)
        value_surface = fonts["bold_26"].render(value, True, COLOR_TEXT)
        screen.blit(value_surface, (x + 104, y - 4))

    def _layout(self, screen, game):
        screen_width, screen_height = screen.get_size()
        sidebar_width = min(SIDEBAR_WIDTH, max(240, screen_width // 3))
        board_area_width = max(1, screen_width - sidebar_width)
        sidebar_rect = pygame.Rect(screen_width - sidebar_width, 0, sidebar_width, screen_height)

        available_width = max(1, board_area_width - BOARD_PADDING * 2)
        available_height = max(1, screen_height - BOARD_PADDING * 2)
        tile_size = min(TILE_SIZE, available_width // game.board_cols, available_height // game.board_rows)
        tile_size = max(24, tile_size)

        board_width = game.board_cols * tile_size
        board_height = game.board_rows * tile_size
        board_x = max(BOARD_PADDING, (board_area_width - board_width) // 2)
        board_y = max(BOARD_PADDING, (screen_height - board_height) // 2)
        board_rect = pygame.Rect(board_x, board_y, board_width, board_height)

        return {
            "tile_size": tile_size,
            "board_rect": board_rect,
            "sidebar_rect": sidebar_rect,
        }

    def _layout_buttons(self, buttons, layout):
        sidebar_rect = layout["sidebar_rect"]
        button_width = max(150, sidebar_rect.width - 80)
        compact = sidebar_rect.height < 620
        button_height = 40 if compact else 46
        gap = 8 if compact else 12
        x = sidebar_rect.x + (sidebar_rect.width - button_width) // 2
        total_height = len(buttons) * button_height + (len(buttons) - 1) * gap
        y = max(224, min(236, sidebar_rect.height - total_height - 92))

        for index, button in enumerate(buttons):
            button.set_rect(x, y + index * (button_height + gap), button_width, button_height)

    def _layout_algorithm_buttons(self, buttons, layout):
        sidebar_rect = layout["sidebar_rect"]
        button_width = max(150, sidebar_rect.width - 92)
        button_height = 42
        gap = 10
        total_height = len(buttons) * button_height + (len(buttons) - 1) * gap
        x = sidebar_rect.x + (sidebar_rect.width - button_width) // 2
        y = max(142, (sidebar_rect.height - total_height) // 2 + 40)

        for index, button in enumerate(buttons):
            button.set_rect(x, y + index * (button_height + gap), button_width, button_height)

    def _draw_algorithm_menu(self, screen, buttons, mouse_pos, layout):
        overlay = pygame.Surface(screen.get_size(), pygame.SRCALPHA)
        overlay.fill((0, 0, 0, 150))
        screen.blit(overlay, (0, 0))

        sidebar_rect = layout["sidebar_rect"]
        menu_rect = pygame.Rect(sidebar_rect.x + 22, 104, sidebar_rect.width - 44, min(430, sidebar_rect.height - 142))
        pygame.draw.rect(screen, COLOR_SIDEBAR, menu_rect, border_radius=12)
        pygame.draw.rect(screen, COLOR_BOARD_BORDER, menu_rect, width=2, border_radius=12)

        fonts = self.assets.fonts
        self._draw_centered_text(screen, "CHOOSE SOLVER", fonts["bold_26"], COLOR_TEXT, menu_rect.centerx, menu_rect.y + 24)
        self._draw_centered_text(screen, "A Star calls the C++ solver", fonts["regular_18"], COLOR_MUTED, menu_rect.centerx, menu_rect.y + 58)

        for button in buttons:
            button.draw(screen, mouse_pos)
