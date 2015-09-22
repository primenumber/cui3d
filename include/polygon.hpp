#ifndef _HEADER_CUI3D_POLYGON_HPP_
#define _HEADER_CUI3D_POLYGON_HPP_
#include "cui3d.hpp"
#include <cstddef>
#include <array>
#include <vector>
#include <boost/optional.hpp>
#include "geometry.hpp"
#include "texture.hpp"

namespace cui3d {

struct Polygon {
 public:
  template <typename T>
  using table = std::vector<std::vector<T>>;
  std::vector<Triangle> triangles;
  Texture texture;
  Polygon() : texture(defaultTexture) {}
  Polygon(const Polygon &) = default;
  Polygon(Polygon &&) = default;
};

Polygon make_cuboid(const Vec3D &, const Vec3D &);
Polygon applyTransform(const Polygon &, const Transform3D &);

class Camera {
 public:
  Camera() : camera_pos(0, 0, -1.0), camera_direction(0, 0, 1.0) {};
  CuiImage render(CuiImage &, const std::vector<Polygon> &);
  Vec3D camera_pos;
  Vec3D camera_direction;
 private:
  boost::optional<Pixel> render_impl(const std::vector<Polygon> &,
      const double, const double, const double,
      const std::array<Vec3D, 3> &);
};

} // namespace cui3d
#endif
