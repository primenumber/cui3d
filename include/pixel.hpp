#ifndef _HEADER_CUI3D_PIXEL_HPP_
#define _HEADER_CUI3D_PIXEL_HPP_

namespace cui3d {

enum class Color {
  BLACK,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  YELLOW,
  WHITE
};

struct Pixel {
 public:
  char ch;
  Color foreground_color;
  Color background_color;
  constexpr Pixel()
    : ch(' '), foreground_color(Color::WHITE), background_color(Color::BLACK) {}
  constexpr Pixel(const Pixel &) = default;
  constexpr Pixel(const char ch, const Color fgclr, const Color bgclr)
    : ch(ch), foreground_color(fgclr), background_color(bgclr) {}
  Pixel &operator=(const Pixel &) = default;
};

inline bool operator==(const Pixel &lhs, const Pixel &rhs) {
  return lhs.ch == rhs.ch && lhs.foreground_color == rhs.foreground_color &&
    lhs.background_color == rhs.background_color;
}

inline bool operator!=(const Pixel &lhs, const Pixel &rhs) {
  return lhs.ch != rhs.ch || lhs.foreground_color != rhs.foreground_color || 
    lhs.background_color != rhs.background_color;
}

} // namespace cui3d

#endif
