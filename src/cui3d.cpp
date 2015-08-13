#include "cui3d.hpp"
#include <algorithm>
#include <iostream>
#include <ncurses.h>

namespace cui3d {

void CuiImage::view() {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if (visible[i][j]) std::cout << data[i][j];
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
        addch(next_image.data[i][j]);
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
