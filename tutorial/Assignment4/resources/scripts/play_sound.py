import pygame
import sys
import time
pygame.init()
pygame.mixer.init()
pygame.mixer.music.load(sys.argv[1])
pygame.mixer.music.set_volume(0.6)
pygame.mixer.music.play(-1)
start_time = time.time()
while pygame.mixer.music.get_busy():
	if time.time() - start_time > int(sys.argv[2]) - 0.5:
		break
	pygame.time.Clock().tick(10)
pygame.quit()