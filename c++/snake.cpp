/******************************************************************************

   @file    snake.cpp
   @author  Rajmund Szymanski
   @date    14.04.2020
   @brief   Sudoku game and generator

*******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

******************************************************************************/

#include "console.hpp"

#define BKG Console::Green

const int Width  = 40;
const int Height = 30;

//----------------------------------------------------------------------------

class Game : public Console
{
	void update();

public:

	const int width, height;

	Game() : Console("SNAKE!", 100), width(Width), height(Height)
	{
		SetFont(32, L"Consolas");
		Center(width, height / 2);
		Clear(BKG, BKG);
		HideCursor();
	}

	~Game()
	{
		Clear(Console::Default);
	}

	void show();
};

Game con;

//----------------------------------------------------------------------------

struct Point : POINT
{
	Point( int _x, int _y ) : POINT { _x, _y } {}

	void moveTo( int _x, int _y )
	{
		x = _x;
		y = _y;
	}

	void put( Console::Color color )
	{
		Console::Color fore, back;
		con.Get(x, y / 2, &fore, &back);
		if (y % 2 == 0) {
			fore = color;
		}
		else {
			if (fore == Console::White)
				fore = BKG; // fruit or snake over info text
			back = color;
		}
		con.Put(x, y / 2, '\xDF', fore, back);
	}
};

typedef std::list<Point> Body;

//----------------------------------------------------------------------------

struct Fruit : Point
{
	int value;
	int delay;

	Fruit() : Point(0, 0)
	{
		srand(time(0));
		create();
	}

	void create()
	{
		moveTo(rand() % con.width, rand() % con.height);
		value =  9;
		delay = 20;
	}

	void move()
	{
		if (--delay <= 0) { delay = 20; if (value > 0) value--; }
	}

	void update()
	{
		put((delay <= 10) ? Console::Yellow : Console::Red);
	}
};

Fruit fruit;

//----------------------------------------------------------------------------

enum Dir
{
	Up = 0,
	Right,
	Down,
	Left,
};

//----------------------------------------------------------------------------

struct Snake : Point
{
	Body  body;
	Dir   direction;
	int   length;
	bool  alive;

	Snake(): Point((con.width + 1) / 2, con.height - 1), direction(Up), length(3), alive(true)
	{
	}

	int size() const
	{
		return body.size();
	}

	void turnLeft()
	{
		direction = static_cast<Dir>((direction + 3) % 4);
	}

	void turnRight()
	{
		direction = static_cast<Dir>((direction + 1) % 4);
	}

	void move()
	{
		body.push_front(*this);

		if (size() > length) {
			body.back().put(BKG);
			body.pop_back();
		}

		switch (direction) {
			case  Up:    if (--y <  0         ) alive = false; break;
			case  Right: if (++x >= con.width ) alive = false; break;
			case  Down:  if (++y >= con.height) alive = false; break;
			case  Left:  if (--x <  0         ) alive = false; break;
		}

		for (auto &i: body) {
			if (gulp(i)) {
				alive = false;
				break;
			}
		}
	}

	void update()
	{
		put(Console::Blue);
		for (auto &i: body)
			i.put(Console::Grey);
	}

	bool gulp( Point &p )
	{
		return (x == p.x) && (y == p.y);
	}

	void rise( int count )
	{
		length += count;
	}
};

Snake snake;

//----------------------------------------------------------------------------

void Game::update()
{
	if (con.wait()) return;

	snake.move();
	fruit.move();

	if (snake.gulp(fruit)) {
		snake.rise(fruit.value);
		fruit.create();
	}

	char text[8] = { 0 };
	wsprintf(text, "%d", fruit.value);
	con.Put(1, 0, text, Console::White, BKG);
	wsprintf(text, "%4d", snake.size());
	con.Put(con.width - 5, 0, text, Console::White, BKG);

	snake.update();
	fruit.update();
}

void Game::show()
{
	INPUT_RECORD input;

	while (snake.alive) {
		update();
		if (con.GetInput(&input) && input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown == TRUE) {
			int key = input.Event.KeyEvent.wVirtualKeyCode;
			switch (key) {
				case VK_LEFT:   snake.turnLeft();  break;
				case VK_RIGHT:  snake.turnRight(); break;
				case VK_ESCAPE: return;
			}
		}
	}

	const char *text = " GAME OVER ";
	int x = (con.width - std::strlen(text)) / 2;
	int y = con.height / 4;
	con.Put(x, y, text, Console::White, Console::Red);
	con.DrawBold(x - 1, y - 1, 13, 3);
	con.ColorFrame(x - 1, y - 1, 13, 3, Console::Red, BKG);
	Sleep(2000);
}

//----------------------------------------------------------------------------

int main()
{
	con.show();
	return 0;
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	con.show();
	return 0;
}
