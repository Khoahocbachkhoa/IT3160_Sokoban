import pygame

from config import (
    FONT_BOLD,
    FONT_REGULAR,
    IMAGE_BOX,
    IMAGE_GROUND,
    IMAGE_PLAYER,
    IMAGE_TARGET,
    IMAGE_WALL,
    TILE_SIZE,
)


class AssetManager:
    def __init__(self):
        self.images = self._load_images()
        self._scaled_cache = {}
        self.fonts = self._load_fonts()

    def _load_image(self, path):
        image = pygame.image.load(str(path)).convert_alpha()
        return pygame.transform.smoothscale(image, (TILE_SIZE, TILE_SIZE))

    def _load_images(self):
        return {
            "player": self._load_image(IMAGE_PLAYER),
            "target": self._load_image(IMAGE_TARGET),
            "box": self._load_image(IMAGE_BOX),
            "wall": self._load_image(IMAGE_WALL),
            "ground": self._load_image(IMAGE_GROUND),
        }

    def _load_fonts(self):
        return {
            "regular_18": pygame.font.Font(str(FONT_REGULAR), 18),
            "regular_20": pygame.font.Font(str(FONT_REGULAR), 20),
            "regular_22": pygame.font.Font(str(FONT_REGULAR), 22),
            "bold_22": pygame.font.Font(str(FONT_BOLD), 22),
            "bold_26": pygame.font.Font(str(FONT_BOLD), 26),
            "bold_42": pygame.font.Font(str(FONT_BOLD), 42),
        }

    def image(self, name, size):
        key = (name, size)
        if key not in self._scaled_cache:
            self._scaled_cache[key] = pygame.transform.smoothscale(self.images[name], (size, size))
        return self._scaled_cache[key]
