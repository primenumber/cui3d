#include "cui3d.hpp"
#include <algorithm>
#include <iostream>
#include <ncurses.h>

namespace cui3d {

void CuiImage::view() {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if (visible[i][j]) std::cout << data[i][j].ch;
      else std::cout << ' ';
    }
    std::cout << std::endl;
  }
}

CuiImage &composite(CuiImage &lhs, const CuiImage &rhs) {
  for (int i = 0; i < std::min(lhs.height, rhs.height); ++i) {
    for (int j = 0; j < std::min(lhs.width, rhs.width); ++j) {
      if (rhs.visible[i][j]) {
        lhs.visible[i][j] = true;
        lhs.data[i][j] = rhs.data[i][j];
      }
    }
  }
  return lhs;
}

bool Screen::is_init_scr = false;

Screen::Screen() {
  if (!is_init_scr) {
    initscr();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    is_init_scr = true;
  }
  getmaxyx(stdscr, height, width);
  current_image = CuiImage(height, width);
  next_image = current_image;
}

void Screen::draw(const CuiImage &img) {
  composite(next_image, img);
}

void Screen::render() {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if (next_image.visible[i][j]) {
        if (current_image.visible[i][j]
            && next_image.data[i][j] == current_image.data[i][j]) continue;
        move(i, j);
        attrset(COLOR_PAIR(next_image.data[i][j].foreground_color));
        addch(next_image.data[i][j].ch);
      } else if (current_image.visible[i][j]) {
        move(i, j);
        addch(' ');
      }
    }
  }
  refresh();
  current_image = std::move(next_image);
  next_image = CuiImage(height, width);
}

Screen::~Screen() {
  endwin();
  is_init_scr = false;
}

} // namespace cui3d
