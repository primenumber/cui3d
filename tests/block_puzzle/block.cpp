#include "block.hpp"

#include <algorithm>

I3d operator+(const I3d &lhs, const I3d &rhs) {
  I3d res;
  for (int i = 0; i < 3; ++i)
    res[i] = rhs[i] + lhs[i];
  return res;
}
I3d &operator+=(I3d &lhs, const I3d &rhs) {
  return lhs = lhs + rhs;
}
I3d operator-(const I3d &lhs, const I3d &rhs) {
  I3d res;
  for (int i = 0; i < 3; ++i)
    res[i] = lhs[i] - rhs[i];
  return res;
}
I3d &operator-=(I3d &lhs, const I3d &rhs) {
  return lhs = lhs - rhs;
}
int norm(const I3d &v) {
  int sum = 0;
  for (int i : v) sum += i*i;
  return sum;
}

Block move(const Block &b, I3d delta) {
  Block res = b;
  for (int i = 0; i < 3; ++i)
    res.offset[i] += delta[i];
  return res;
}
Block rotateX(const Block &b) {
  Block res;
  res.offset = b.offset;
  for (I3d c : b.cubes) {
    I3d resc;
    resc[0] = c[0];
    resc[1] = c[2];
    resc[2] = -c[1];
    res.cubes.emplace_back(resc);
  }
  return res;
}
Block rotateY(const Block &b) {
  Block res;
  res.offset = b.offset;
  for (I3d c : b.cubes) {
    I3d resc;
    resc[1] = c[1];
    resc[2] = c[0];
    resc[0] = -c[2];
    res.cubes.emplace_back(resc);
  }
  return res;
}
Block rotateZ(const Block &b) {
  Block res;
  res.offset = b.offset;
  for (I3d c : b.cubes) {
    I3d resc;
    resc[2] = c[2];
    resc[0] = c[1];
    resc[1] = -c[0];
    res.cubes.emplace_back(resc);
  }
  return res;
}

bool is_conflict(const Block &lhs, const Block &rhs) {
  using std::begin;
  using std::end;
  using std::find;
  I3d diff = rhs.offset - lhs.offset;
  for (I3d c : rhs.cubes) {
    c += diff;
    if (find(begin(lhs.cubes), end(lhs.cubes), c) != end(lhs.cubes))
      return true;
  }
  return false;
}
Block merge(const Block &lhs, const Block &rhs) {
  Block res = lhs;
  I3d diff = rhs.offset - lhs.offset;
  for (I3d c : rhs.cubes) {
    c += diff;
    res.cubes.push_back(c);
  }
  return res;
}
bool is_subset(const Block &lhs, const Block &rhs) {
  using std::begin;
  using std::end;
  using std::find;
  I3d diff = rhs.offset - lhs.offset;
  for (I3d c : lhs.cubes) {
    c += diff;
    if (find(begin(rhs.cubes), end(rhs.cubes), c) == end(rhs.cubes))
      return false;
  }
  return true;
}

bool operator==(const Block &lhs, const Block &rhs) {
  return is_subset(lhs, rhs) && is_subset(rhs, lhs);
}
bool operator!=(const Block &lhs, const Block &rhs) {
  return !(lhs == rhs);
}

cui3d::Polygon to_polygon(const Block &blk, const cui3d::Texture &texture) {
  using std::begin;
  using std::end;
  cui3d::Polygon poly;
  poly.texture = texture;
  for (I3d c : blk.cubes) {
    c += blk.offset;
    auto cube = cui3d::make_cuboid(
        0.1*cui3d::Vec3D(c[0], c[1], c[2]),
        0.1*cui3d::Vec3D(c[0]+1, c[1]+1, c[2]+1));
    poly.triangles.insert(end(poly.triangles),
        begin(cube.triangles), end(cube.triangles));
  }
  return poly;
}

I3d gravity_point(const std::vector<I3d> &i3dv) {
  I3d sum = {0, 0, 0};
  for (I3d i : i3dv) sum += i;
  int n = i3dv.size();
  return {sum[0]/n, sum[1]/n, sum[2]/n};
}

std::vector<Block> divide_block(const Block &blk, int num) {
  while (true) {
    std::vector<Block> res(num);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dis(0, num-1);
    for (I3d c : blk.cubes) {
      res[dis(mt)].cubes.push_back(c);
    }
    bool ok = true;
    for (Block &resb : res) {
      if (resb.cubes.empty()) {
        ok = false;
        break;
      }
      resb.offset = gravity_point(resb.cubes);
      for (I3d &cube : resb.cubes)
        cube -= resb.offset;
    }
    if (ok) return res;
  }
}
