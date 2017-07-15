#include "block.hpp"
#include <random>
#include <algorithm>
#include <queue>
#include <set>

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

class UnionFind {
 public:
  UnionFind(const std::size_t size) : parent(size, -1) {}
  int root(const int index) {
    if (parent[index] < 0) return index;
    return parent[index] = root(parent[index]);
  }
  int join(const int index1, const int index2) {
    int r1 = root(index1);
    int r2 = root(index2);
    if (r1 == r2) return false;
    if (parent[r1] < parent[r2]) std::swap(r1, r2);
    parent[r2] += parent[r1];
    parent[r1] = r2;
    return true;
  }
  int size() const { return parent.size(); }
  int size(const int index) { return -parent[root(index)]; }
 private:
  std::vector<int> parent;
};
std::vector<std::pair<int, int>> neighbor_index_pairs(const std::vector<I3d> &cubes) {
  std::vector<std::pair<int, int>> res;
  for (std::size_t i = 0; i < cubes.size(); ++i)
    for (std::size_t j = 0; j < i; ++j)
      if (norm(cubes[i] - cubes[j]) == 1)
        res.emplace_back(i, j);
  return res;
}
std::vector<int> classify(std::vector<std::pair<int, int>> edges,
    const int n, const int num) {
  std::random_device rd;
  std::mt19937 mt(rd());
  UnionFind uf(n);
  shuffle(begin(edges), end(edges), mt);
  int conti_num = n;
  for (auto e : edges) {
    if (uf.size(e.first) > n/num || uf.size(e.second) > n/num) continue;
    if (uf.join(e.first, e.second)) {
      --conti_num;
      if (conti_num == num) break;
    }
  }
  std::vector<int> roots;
  for (int i = 0; i < n; ++i)
    roots.push_back(uf.root(i));
  std::sort(begin(roots), end(roots));
  roots.erase(std::unique(begin(roots), end(roots)), end(roots));
  std::vector<int> res;
  for (int i = 0; i < n; ++i) {
    int index = lower_bound(begin(roots), end(roots), uf.root(i)) - begin(roots);
    res.push_back(index);
  }
  return res;
}
std::vector<Block> divide_block(const Block &blk, const int num) {
  using std::begin;
  using std::end;
  std::vector<Block> res(num);
  const int n = blk.cubes.size();
  auto edges = neighbor_index_pairs(blk.cubes);
  auto indices = classify(edges, n, num);
  for (int i = 0; i < n; ++i) {
    res[indices[i]].cubes.push_back(blk.cubes[i]);
  }
  for (Block &resb : res) {
    resb.offset = gravity_point(resb.cubes);
    for (I3d &cube : resb.cubes)
      cube -= resb.offset;
  }
  return res;
}
