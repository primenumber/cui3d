#include "texture.hpp"

namespace cui3d {

Pixel PlaneMappingTexture::operator()(const Vec3D &vec) {
  Vec3D origined_vec = vec - origin;
  return Pixel('@', Color::RED, Color::BLACK);
}

} // namespace cui3d
