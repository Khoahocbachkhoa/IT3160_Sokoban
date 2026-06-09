from config import LEVEL_DIR
from src.level import Level


class Game:
    def __init__(self):
        self.level_number = Level.first_level_number()
        self.level = None
        self.move_count = 0
        self.paused = False
        self.win = False
        self.solution = ""
        self.solution_index = 0
        self.solver_message = ""
        self.load_level(self.level_number)

    @property
    def board_rows(self):
        return self.level.rows

    @property
    def board_cols(self):
        return self.level.cols

    def load_level(self, level_number):
        self.level_number = level_number
        self.level = Level.from_number(level_number)
        self.move_count = 0
        self.paused = False
        self.win = self.is_completed()
        self.solution = ""
        self.solution_index = 0
        self.solver_message = ""

    def retry(self):
        self.load_level(self.level_number)

    def next_level(self):
        self.load_level(Level.next_level_number(self.level_number))

    def toggle_pause(self):
        if not self.win:
            self.paused = not self.paused

    def move_player(self, d_row, d_col):
        if self.paused or self.win:
            return False

        row, col = self.level.player_pos
        next_row = row + d_row
        next_col = col + d_col

        if not self._inside(next_row, next_col):
            return False

        next_cell = self.level.grid[next_row][next_col]

        if self.is_wall(next_cell):
            return False

        if self.is_free(next_cell):
            self._move_into(row, col, next_row, next_col)
            return True

        if self.is_box(next_cell):
            box_row = next_row + d_row
            box_col = next_col + d_col

            if not self._inside(box_row, box_col):
                return False

            box_target_cell = self.level.grid[box_row][box_col]
            if not self.is_free(box_target_cell):
                return False

            self.level.grid[box_row][box_col] = self.place_box_on_cell(box_target_cell)
            self.level.grid[next_row][next_col] = self.clear_box_cell(next_cell)
            self._move_into(row, col, next_row, next_col)
            return True

        return False

    def apply_action(self, action):
        directions = {
            "U": (-1, 0),
            "D": (1, 0),
            "L": (0, -1),
            "R": (0, 1),
        }
        direction = directions.get(action.upper())
        if direction is None:
            return False
        return self.move_player(*direction)

    def set_solution(self, solution):
        self.solution = solution
        self.solution_index = 0

    def step_solution(self):
        if not self.solution or self.solution_index >= len(self.solution):
            return False

        moved = self.apply_action(self.solution[self.solution_index])
        if moved:
            self.solution_index += 1
        return moved

    def is_completed(self):
        for row in self.level.grid:
            if "$" in row:
                return False
        return True

    def status_text(self):
        if self.win:
            return "YOU WIN"
        if self.paused:
            return "PAUSED"
        return "PLAYING"

    def is_wall(self, cell):
        return cell == "#"

    def is_box(self, cell):
        return cell in ("$", "*")

    def is_free(self, cell):
        return cell in (" ", ".")

    def clear_player_cell(self, cell):
        if cell == "@":
            return " "
        if cell == "+":
            return "."
        return cell

    def place_player_on_cell(self, cell):
        if cell == " ":
            return "@"
        if cell == ".":
            return "+"
        return cell

    def clear_box_cell(self, cell):
        if cell == "$":
            return " "
        if cell == "*":
            return "."
        return cell

    def place_box_on_cell(self, cell):
        if cell == " ":
            return "$"
        if cell == ".":
            return "*"
        return cell

    def _move_into(self, row, col, next_row, next_col):
        current_cell = self.level.grid[row][col]
        target_cell = self.level.grid[next_row][next_col]

        self.level.grid[row][col] = self.clear_player_cell(current_cell)
        self.level.grid[next_row][next_col] = self.place_player_on_cell(target_cell)
        self.level.player_pos = [next_row, next_col]
        self.move_count += 1
        self.win = self.is_completed()

    def _inside(self, row, col):
        return 0 <= row < self.level.rows and 0 <= col < self.level.cols
