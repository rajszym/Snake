# coding: CP852

import os, sys, random, pygame
from pygame.locals import *

White  = (0xFF, 0xFF, 0xFF)
Red    = (0xFF, 0x00, 0x00)
Yellow = (0xFF, 0xFF, 0x00)
Green  = (0x00, 0x80, 0x00)
Azure  = (0x40, 0x80, 0xFF)
Blue   = (0x00, 0x00, 0x80)
Black  = (0x00, 0x00, 0x00)
BKG    = Green

class Surface(pygame.Surface):

	def drawCell(self, margin, color, fill = True):

		pygame.draw.rect(self, color, (margin, margin, Cell - 2 * margin, Cell - 2 * margin), not fill)

class Snake:

	def __init__(self):

		self.head = [(Width // 2) * Cell, (Height - 1) * Cell]
		self.body = []

		self.himg = Surface(CellSize)
		self.bimg = Surface(CellSize)

		self.himg.fill(Blue)
		self.bimg.fill(BKG)
		self.bimg.drawCell(1, Azure)
		self.bimg.drawCell(4, Blue)

		self.length = 3
		self.angle  = 0
		self.alive  = True

	def turn_left(self):

		self.angle = (self.angle + 3) % 4

	def turn_right(self):

		self.angle = (self.angle + 1) % 4

	def move(self):

		self.body.insert(0, self.head)
		self.body = self.body[:self.length]
		self.head = self.head[:]
		if   self.angle == 0: self.head[1] -= Cell
		elif self.angle == 1: self.head[0] += Cell
		elif self.angle == 2: self.head[1] += Cell
		elif self.angle == 3: self.head[0] -= Cell

		if self.head in self.body or self.head[0] < 0 or self.head[0] >= ScreenWidth or self.head[1] < 0 or self.head[1] >= ScreenHeight:
			self.alive = False

	def update(self):

		screen = pygame.display.get_surface()
		screen.blit(self.himg, self.head)
		for b in self.body: screen.blit(self.bimg, b)

class Fruit:

	def __init__(self):

		self.bkg = Surface(CellSize); self.bkg.fill(BKG)
		self.bkg.drawCell(1, Yellow, False)
		self.img = Surface(CellSize); self.img.fill(BKG)
		self.img.drawCell(1, Yellow, False)
		self.img.drawCell(4, Red)
		self.move()

	def move(self):

		self.score = 9
		self.ticks = pygame.time.get_ticks() + 1000
		self.pos = [random.randint(0, Width - 1) * Cell, random.randint(0, Height - 1) * Cell]

	def update(self):

		if self.score > 0 and pygame.time.get_ticks() >= self.ticks: self.score -= 1; self.ticks += 1000
		screen = pygame.display.get_surface()
		screen.blit(self.img if pygame.time.get_ticks() % 1024 < 512 else self.bkg, self.pos)

def showInfo(s, l):

	screen = pygame.display.get_surface()
	font = pygame.font.SysFont("Consolas", 24)
#	font = pygame.font.Font(None, 32)

	text = font.render(str(s), 1, White)
	rect = text.get_rect()
	rect = rect.move(4, 4)
	screen.blit(text, rect)

	text = font.render(str(l), 1, White)
	rect = text.get_rect()
	rect = rect.move(ScreenWidth - rect.right - 4, 4)
	screen.blit(text, rect)

def showEnd():

	screen = pygame.display.get_surface()
	font = pygame.font.SysFont("Consolas", 48)
#	font = pygame.font.Font(None, 64)

	text = font.render("GAME OVER", 1, White)
	rect = text.get_rect(); rect.center = ScreenWidth // 2, ScreenHeight // 2
	screen.blit(text, rect)

	pygame.display.update()
	pygame.time.wait(2000)

def main():
    
	screen = pygame.display.set_mode(ScreenSize)
	snake  = Snake()
	fruit  = Fruit()

	while snake.alive:

		for e in pygame.event.get():
			if e.type == QUIT or e.type == KEYDOWN and e.key == K_ESCAPE: return
			if e.type == KEYDOWN:
				if e.key == K_LEFT:  snake.turn_left()
				if e.key == K_RIGHT: snake.turn_right()
		snake.move()
		if fruit.pos in snake.body: snake.length += fruit.score; fruit.move()
		screen.fill(BKG)
		snake.update()
		fruit.update()
		showInfo(fruit.score, len(snake.body))
		pygame.display.update()
		pygame.time.wait(100)

	showEnd()

CellSize = Cell, Cell = 16, 16
Size = Width, Height = 40, 30
ScreenSize = ScreenWidth, ScreenHeight = Width * Cell, Height * Cell

if __name__ == "__main__":

	os.environ["SDL_VIDEO_CENTERED"] = "1"
#	os.environ["SDL_VIDEODRIVER"]    = "windib"
	pygame.init()
	pygame.display.set_caption("SNAKE!")
	main()
	pygame.quit()
