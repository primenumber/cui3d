#pragma once
#include <array>
#include <vector>
#include <texture.hpp>
#include <polygon.hpp>

using I3d = std::array<int, 3>;

I3d operator+(const I3d &, const I3d &);
I3d &operator+=(I3d &, const I3d &);
I3d operator-(const I3d &, const I3d &);
I3d &operator-=(I3d &, const I3d &);
int norm(const I3d &);

struct Block {
  std::vector<I3d> cubes;
  I3d offset;
};

Block move(const Block &, I3d);

Block rotateX(const Block &);
Block rotateY(const Block &);
Block rotateZ(const Block &);
bool is_conflict(const Block &, const Block &);
Block merge(const Block &, const Block &);
// return true if lhs is subset of rhs
bool is_subset(const Block &lhs, const Block &rhs);
bool operator==(const Block &, const Block &);
bool operator!=(const Block &, const Block &);
cui3d::Polygon to_polygon(const Block &, const cui3d::Texture &);
std::vector<Block> divide_block(const Block &, int);
