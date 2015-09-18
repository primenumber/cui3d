#ifndef _HEADER_CUI3D_CUI3D_HPP_
#define _HEADER_CUI3D_CUI3D_HPP_
#include "status.hpp"
#include <cstddef>
#include <vector>
#include "pixel.hpp"

namespace cui3d {

class CuiImage {
  template <typename T>
  using table = std::vector<std::vector<T>>;
 public:
  CuiImage() : height(0), width(0) {}
  CuiImage(std::size_t height, std::size_t width)
    : height(height), width(width),
      data(height, std::vector<Pixel>(width)),
      visible(height, std::vector<bool>(width, false)) {};
  CuiImage(std::size_t height, std::size_t width,
      const table<Pixel> &data, const table<bool> &visible)
    : height(height), width(width),
      data(data), visible(visible) {};
  void view();
  std::size_t height, width;
  table<Pixel> data;
  table<bool> visible;
 private:
};

CuiImage &composite(CuiImage &, const CuiImage &);

class Screen {
 public:
  Screen();
  ~Screen();
  void draw(const CuiImage &);
  void render();
  void clear();
  std::size_t get_height() const { return height; }
  std::size_t get_width() const { return width; }
 private:
  CuiImage current_image;
  CuiImage next_image;
  std::size_t height;
  std::size_t width;
  static bool is_init_scr;
};

} // namespace cui3d

#include "polygon.hpp"
#endif
