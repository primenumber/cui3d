#include "texture.hpp"

namespace cui3d {

Texture defaultTexture =
  [](const Vec3D &) { return Pixel('|', Color::WHITE, Color::BLACK); };

Pixel PlaneMappingTexture::operator()(const Vec3D &vec) {
  Vec3D origined_vec = vec - origin;
  return Pixel('@', Color::RED, Color::WHITE);
}

} // namespace cui3d
