#include "render.hpp"

/*
 * Sets the position of the cursor to pos
 */
void Renderer::setCursorPos(Coord pos) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD)pos);
};

/*
 * Sets the position of the cursor to pos
 */
Coord Renderer::getCursorPos() {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

	return Coord(info.dwCursorPosition);
};

/*
 * Sets the term color to color
 */
void Renderer::setTermColor(unsigned char color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
};

/*
 * Sets the term color to light gray
 */
void Renderer::resetTermColor() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), TermColor::GRAY);
};
