import pygame

from config import COLOR_BUTTON, COLOR_BUTTON_BORDER, COLOR_BUTTON_HOVER, COLOR_BUTTON_TEXT


class Button:
    def __init__(self, x, y, width, height, text, font, action=None):
        self.rect = pygame.Rect(x, y, width, height)
        self.text = text
        self.font = font
        self.action = action or text.lower()

    def set_rect(self, x, y, width, height):
        self.rect.update(x, y, width, height)

    def draw(self, screen, mouse_pos):
        is_hovered = self.rect.collidepoint(mouse_pos)
        color = COLOR_BUTTON_HOVER if is_hovered else COLOR_BUTTON

        pygame.draw.rect(screen, color, self.rect, border_radius=8)
        pygame.draw.rect(screen, COLOR_BUTTON_BORDER, self.rect, width=2, border_radius=8)

        label = self.font.render(self.text, True, COLOR_BUTTON_TEXT)
        label_rect = label.get_rect(center=self.rect.center)
        screen.blit(label, label_rect)

    def is_clicked(self, mouse_pos):
        return self.rect.collidepoint(mouse_pos)
