require 'gosu'

White  = Gosu::Color.argb(0xFF_FFFFFF)
Red    = Gosu::Color.argb(0xFF_FF0000)
Yellow = Gosu::Color.argb(0xFF_FFFF00)
Green  = Gosu::Color.argb(0xFF_008000)
Azure  = Gosu::Color.argb(0xFF_4080FF)
Blue   = Gosu::Color.argb(0xFF_000080)
Black  = Gosu::Color.argb(0xFF_000000)
BKG    = Green

module ZOrder
	BACKGROUND, SNAKE, FRUIT, TEXT = *0..3
end

def draw_cell(x, y, margin, color, zorder)
	Gosu::draw_rect(x + margin, y + margin, Cell - 2 * margin, Cell - 2 * margin, color, zorder)
end

class Array
	def out_of_size?(size)
		each_with_index { |x, i| return true if x < 0 or x >= size[i] }
		false
	end
end

class Snake

	def initialize
		@head = [(Width / 2) * Cell, (Height - 1) * Cell]
		@body = []

		@length = 3
		@angle  = 0
		@alive  = true
	end

	def turn_left
		@angle = (@angle + 3) % 4
	end

	def turn_right
		@angle = (@angle + 1) % 4
	end

	def move
		@body.unshift(@head.dup)
		@body.pop if @body.length > @length
#		@body = @body[0, @length]

		case @angle
		when 0 then @head[1] -= Cell
		when 1 then @head[0] += Cell
		when 2 then @head[1] += Cell
		when 3 then @head[0] -= Cell
		end

		@alive = false if @body.include?(@head) or @head.out_of_size?(ScreenSize)
	end

	def update
		move
		if gulp?($fruit)
			rise
			$fruit.create
		end
	end
	
	def draw
		draw_cell(*@head, 0, Blue,  ZOrder::SNAKE)
		@body.each do |b|
			draw_cell(*b, 0, BKG,   ZOrder::SNAKE)
			draw_cell(*b, 1, Azure, ZOrder::SNAKE)
			draw_cell(*b, 4, Blue,  ZOrder::SNAKE)
		end
	end

	def gulp?(obj)
		@head == obj.pos
	end

	def size
		@body.length
	end

	def rise
		@length += $score
	end

	def alive?
		@alive
	end

end

class Fruit

	attr_reader :pos

	def initialize
		create
	end

	def create
		@pos   = [rand(Width) * Cell, rand(Height) * Cell]
		$score = 9
		@timer = Gosu::milliseconds + 1000
	end

	def update
		if Gosu::milliseconds >= @timer
			$score -= 1 if not $score.zero?
			@timer += 1000
		end
	end
	
	def draw
		draw_cell(*@pos, 0, BKG,    ZOrder::FRUIT)
		draw_cell(*@pos, 1, Yellow, ZOrder::FRUIT)
		c = Red
		c.alpha = (Gosu::milliseconds / 4) % 256
		draw_cell(*@pos, 4, c, ZOrder::FRUIT)
	end

end

class Game < Gosu::Window
	
	def initialize
		super(*ScreenSize, false, 100)
		self.caption = 'SNAKE!'
		@font = Gosu::Font.new(24)
		@large = Gosu::Font.new(48)
	end

	def button_down(id)
		close if id == Gosu::KbEscape
		$snake.turn_left  if id == Gosu::KbLeft
		$snake.turn_right if id == Gosu::KbRight
	end

	def update
		if $snake.alive?
			$fruit.update
			$snake.update
			@stop = Gosu::milliseconds + 2000
		end
		close if Gosu::milliseconds >= @stop
	end

	def draw
		Gosu::draw_rect(*ScreenRect, BKG, ZOrder::BACKGROUND)
		$snake.draw
		$fruit.draw
		showInfo
		showEnd if $snake.alive? == false
	end

	private

	def showInfo
		@font.draw_text_rel("#{$score}", 4, 4, ZOrder::TEXT, 0.0, 0.0)
		@font.draw_text_rel("#{$snake.size}", ScreenWidth - 4, 4, ZOrder::TEXT, 1.0, 0.0)
	end

	def showEnd
		@large.draw_text_rel("GAME OVER", ScreenWidth / 2, ScreenHeight / 2, ZOrder::TEXT, 0.5, 0.5)
	end

end

Cell = 16
Size = (Width, Height = 40, 30)
ScreenSize = (ScreenWidth, ScreenHeight = Width * Cell, Height * Cell)
ScreenRect = 0, 0, *ScreenSize

if __FILE__ == $0

	$score = 0
	$snake = Snake.new
	$fruit = Fruit.new
	$game  = Game.new
	$game.show

end
