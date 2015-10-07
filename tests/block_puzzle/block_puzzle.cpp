#include <cassert>
#include <ctime>
#include <iostream>
#include <tuple>
#include <sys/time.h>
#include <boost/timer/timer.hpp>
#include <curses.h>
#include <cui3d.hpp>
#include "block.hpp"

class Game {
 public:
  Game(const Block &goal, int n)
    : n(n), now_selected(0), goal(goal), blocks(divide_block(goal, n)) {
    assert(n > 0);
  }
  void select_next() { now_selected = (now_selected + 1) % n; }
  bool move_cmd(char command) {
    if (command == '\n') {
      select_next();
      return true;
    }
    auto nx = now_selected_block();
    switch (command) {
     case 'a':
      nx = move(now_selected_block(), I3d{-1, 0, 0});
      break;
     case 'w':
      nx = move(now_selected_block(), I3d{0, -1, 0});
      break;
     case 's':
      nx = move(now_selected_block(), I3d{0, 1, 0});
      break;
     case 'd':
      nx = move(now_selected_block(), I3d{1, 0, 0});
      break;
     case 'z':
      nx = move(now_selected_block(), I3d{0, 0, 1});
      break;
     case 'x':
      nx = move(now_selected_block(), I3d{0, 0, -1});
      break;
     case 'h':
      nx = rotateX(now_selected_block());
      break;
     case 'j':
      nx = rotateY(now_selected_block());
      break;
     case 'k':
      nx = rotateZ(now_selected_block());
      break;
     default:
      return false;
    }
    if (is_ok(nx)) {
      now_selected_block() = nx;
      return true;
    }
    return false;
  }
  bool is_goal() const {
    Block merged_block;
    for (const Block &b : blocks)
      merge(merged_block, b);
    return merged_block == goal;
  }
  const Block &now_selected_block() const { return blocks[now_selected]; }
  cui3d::CuiImage draw(const cui3d::Camera & cam,
      cui3d::CuiImage &img) const {
    std::vector<cui3d::Polygon> polys;
    int counter = 1;
    for (auto &blk : blocks) {
      polys.emplace_back(to_polygon(blk,
            cui3d::FillfullTexture(cui3d::Pixel(
                ' ',
                cui3d::Color::BLACK,
                static_cast<cui3d::Color>(counter)))));
      ++counter;
    }
    return cam.render(img, polys);
  }
 private:
  int n;
  int now_selected;
  Block goal;
  std::vector<Block> blocks;
  Block &now_selected_block() { return blocks[now_selected]; }
  bool is_ok(const Block &blk) {
    for (int i = 0; i < n; ++i) {
      if (i == now_selected) continue;
      if (is_conflict(blk, blocks[i])) return false;
    }
    return true;
  }
};

std::tuple<int, int> fix_size(int h, int w, double ratio = 2.0) {
  if (w > h * ratio) return std::make_tuple(h, h*ratio);
  else return std::make_tuple(w/ratio, w);
}

cui3d::CuiImage draw(const Game &g, const cui3d::Camera &c, int h, int w) {
  using namespace cui3d;
  CuiImage img(h, w);
  return g.draw(c, img);
}

Game gen_game() {
  Block goal;
  goal.offset = {0, 0, 0};
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      for (int k = 0; k < 3; ++k) {
        goal.cubes.emplace_back(I3d{i-1, j-1, k-1});
      }
    }
  }
  return Game(goal, 5);
}

int main() {
  cui3d::Screen scr;
  int h, w;
  std::tie(h, w) = fix_size(scr.get_height(), scr.get_width(), 2.5);
  Game g = gen_game();
  cui3d::Camera c;
  c.camera_pos = cui3d::Vec3D(0.0, 0.0, -2.0);
  c.camera_direction = cui3d::Vec3D(0.0, 0.0, 2.0);
  scr.draw(draw(g, c, h, w));
  scr.render();
  noecho();
  while (true) {
    int com = getch();
    if (com < 0) break;
    switch (com) {
      using namespace cui3d;
     case 'y':
      c.camera_pos = applyTransform(c.camera_pos, rotateX(pi/6.0));
      c.camera_direction = applyTransform(c.camera_direction, rotateX(pi/6.0));
      break;
     case 'u':
      c.camera_pos = applyTransform(c.camera_pos, rotateY(pi/6.0));
      c.camera_direction = applyTransform(c.camera_direction, rotateY(pi/6.0));
      break;
     case 'i':
      c.camera_pos = applyTransform(c.camera_pos, rotateZ(pi/6.0));
      c.camera_direction = applyTransform(c.camera_direction, rotateZ(pi/6.0));
      break;
     default:
      if(!g.move_cmd(com)) beep();
    }
    scr.draw(draw(g, c, h, w));
    scr.render();
    timespec ts = {0, (long)100000000};
    nanosleep(&ts, nullptr);
  }
  return 0;
}

