#include <cui3d.hpp>
#include <ctime>
#include <iostream>
#include <tuple>
#include <sys/time.h>
#include <boost/timer/timer.hpp>

cui3d::CuiImage draw(int h, int w, double t) {
  using namespace cui3d;
  Camera c;
  c.camera_pos = cui3d::Vec3D(0.0, 0.0, -2.0);
  c.camera_direction = cui3d::Vec3D(0.0, 0.0, 2.0);
  std::vector<Polygon> p;
  p.emplace_back(cui3d::make_cuboid(Vec3D(t*1.6-0.8, 0.2, 0.0),
      Vec3D(t*1.6-0.5,0.5, 0.3)));
  p.emplace_back(cui3d::make_cuboid(Vec3D(-t*1.6+0.2, -0.2, 0.0),
      Vec3D(-t*1.6+0.5,0.1, 0.3)));
  p.emplace_back(applyTransform(cui3d::make_cuboid(Vec3D(-0.15, -0.15, 0.0),
      Vec3D(0.15, 0.15, 0.3)), rotateZ(t)));
  p.emplace_back(applyTransform(cui3d::make_cuboid(Vec3D(-0.45, -0.45, 0.1),
      Vec3D(-0.15, -0.15, 0.4)), rotateX(-t)));
  p[0].texture = PlaneMappingTexture();
  p[1].texture = PlaneMappingTexture();
  p[2].texture = PlaneMappingTexture();
  cui3d::CuiImage img(h, w);
  img = c.render(img, p);
  return img;
}

std::tuple<int, int> fix_size(int h, int w, double ratio = 2.0) {
  if (w > h * ratio) return std::make_tuple(h, h*ratio);
  else return std::make_tuple(w/ratio, w);
}

int main() {
  cui3d::Screen scr;
  for (int i = 0; i < 1200; ++i) {
    double t = i/200.0;
    boost::timer::cpu_timer tm;
    int h, w;
    std::tie(h, w) = fix_size(scr.get_height(), scr.get_width(), 2.5);
    cui3d::CuiImage img = draw(h, w, t);
    scr.draw(img);
    scr.render();
    double rem = 1e+9/60.0 - tm.elapsed().wall;
    if (rem > 0) {
      timespec ts = {0, (long)rem};
      nanosleep(&ts, nullptr);
    }
  }
  return 0;
}
