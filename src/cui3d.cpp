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
    std::array<int, 8> color_ary = {
      COLOR_BLACK,
      COLOR_BLUE,
      COLOR_GREEN,
      COLOR_CYAN,
      COLOR_RED,
      COLOR_MAGENTA,
      COLOR_YELLOW,
      COLOR_WHITE
    };
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j)
        if (i || j)
          init_pair(i * 8 + j, color_ary[i], color_ary[j]);
    assume_default_colors(COLOR_BLACK, COLOR_BLACK);
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
        int fgclr = static_cast<int>(next_image.data[i][j].foreground_color);
        int bgclr = static_cast<int>(next_image.data[i][j].background_color);
        attrset(COLOR_PAIR(fgclr * 8 + bgclr));
        addch(next_image.data[i][j].ch);
      } else if (current_image.visible[i][j]) {
        move(i, j);
        attrset(COLOR_PAIR(0));
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
