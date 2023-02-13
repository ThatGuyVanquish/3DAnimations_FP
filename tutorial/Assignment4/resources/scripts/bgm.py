import pygame
import sys
pygame.init()
pygame.mixer.init()
pygame.mixer.music.load(sys.argv[1])
pygame.mixer.music.set_volume(0.3)
pygame.mixer.music.play(-1)
while pygame.mixer.music.get_busy():
    pygame.time.wait(1000)
