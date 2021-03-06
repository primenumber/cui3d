#ifndef _HEADER_CUI3D_TEXTURE_HPP_
#define _HEADER_CUI3D_TEXTURE_HPP_
#include <functional>
#include "pixel.hpp"
#include "geometry.hpp"

namespace cui3d {

using Texture = std::function<Pixel(const Vec3D &)>;

extern Texture defaultTexture;

class FillfullTexture {
 public:
  const Pixel pixel;
  FillfullTexture(const Pixel &pixel) : pixel(pixel) {}
  Pixel operator()(const Vec3D &) const { return pixel; }
};

class PlaneMappingTexture {
 public:
   std::vector<std::vector<Pixel>> tp;
  Vec3D origin;
  double theta;
  double phi;
  double width;
  double height;
  double rot;
  Pixel operator()(const Vec3D &vec);
};

} // namespace cui3d

#endif
