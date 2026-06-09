import re

from config import LEVEL_DIR


VALID_CELLS = {"#", " ", "$", ".", "@", "*", "+"}


class Level:
    def __init__(self):
        self.grid = []
        self.player_pos = [0, 0]
        self.rows = 0
        self.cols = 0

    def load_from_file(self, path):
        with open(path, "r", encoding="utf-8") as file:
            lines = file.read().splitlines()

        lines = self._trim_empty_edges(lines)
        lines = self._drop_dimension_header(lines)

        if not lines:
            raise ValueError(f"Level file is empty: {path}")

        self.cols = max(len(line) for line in lines)
        self.rows = len(lines)
        self.grid = []

        player_count = 0
        for row_index, line in enumerate(lines):
            row = list(line.ljust(self.cols))
            for col_index, cell in enumerate(row):
                if cell not in VALID_CELLS:
                    row[col_index] = " "
                    cell = " "
                if cell in ("@", "+"):
                    self.player_pos = [row_index, col_index]
                    player_count += 1
            self.grid.append(row)

        if player_count != 1:
            raise ValueError(f"Level must contain exactly one player: {path}")

    @classmethod
    def from_number(cls, level_number):
        level = cls()
        level.load_from_file(LEVEL_DIR / f"level{level_number}.txt")
        return level

    @staticmethod
    def available_level_numbers():
        levels = []
        for path in LEVEL_DIR.glob("level*.txt"):
            match = re.fullmatch(r"level(\d+)\.txt", path.name)
            if match:
                levels.append(int(match.group(1)))
        return sorted(levels)

    @staticmethod
    def first_level_number():
        levels = Level.available_level_numbers()
        if not levels:
            raise FileNotFoundError(f"No level*.txt files found in {LEVEL_DIR}")
        return levels[0]

    @staticmethod
    def next_level_number(current_level):
        levels = Level.available_level_numbers()
        if not levels:
            raise FileNotFoundError(f"No level*.txt files found in {LEVEL_DIR}")

        for level_number in levels:
            if level_number > current_level:
                return level_number
        return levels[0]

    def _trim_empty_edges(self, lines):
        start = 0
        end = len(lines)

        while start < end and lines[start].strip() == "":
            start += 1
        while end > start and lines[end - 1].strip() == "":
            end -= 1

        return lines[start:end]

    def _drop_dimension_header(self, lines):
        if not lines:
            return lines

        parts = lines[0].split()
        if len(parts) == 2 and all(part.isdigit() for part in parts):
            return lines[1:]

        return lines
