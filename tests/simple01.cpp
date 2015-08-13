#include <cui3d.hpp>
#include <ctime>
#include <iostream>
#include <tuple>
#include <sys/time.h>
#include <boost/timer.hpp>

cui3d::CuiImage draw(int h, int w, double t) {
  cui3d::Camera c;
  c.camera_pos = cui3d::Vec3D(0.0, 0.0, -2.0);
  c.camera_direction = cui3d::Vec3D(0.0, 0.0, 1.0);
  std::vector<cui3d::Polygon> poly_v(5);
  /*
  bg.triangles.emplace_back(cui3d::Vec3D(-1.0, -1.0, 0.5),
      cui3d::Vec3D(-1.0, 1.0, 1.0), cui3d::Vec3D(1.0, 1.0, 1.0));
  bg.triangles.emplace_back(cui3d::Vec3D(1.0, 1.0, 1.0),
      cui3d::Vec3D(-1.0, -1.0, 1.0), cui3d::Vec3D(1.0, -1.0, 1.0));
      */
  int x[5] = {0, 1, 1, 0, 0};
  int y[5] = {0, 0, 1, 1, 0};
  for (int i = 0; i < 4; ++i) {
    poly_v[i+1].triangles.emplace_back(
        cui3d::Vec3D(t*0.016+-0.8+x[i]*0.3, t*0.016+-0.8+y[i]*0.3, 0.0),
        cui3d::Vec3D(t*0.016+-0.8+x[i]*0.3, t*0.016+-0.8+y[i]*0.3, 0.3),
        cui3d::Vec3D(t*0.016+-0.8+x[i+1]*0.3, t*0.016+-0.8+y[i+1]*0.3, 0.0));
    poly_v[i+1].triangles.emplace_back(
        cui3d::Vec3D(t*0.016+-0.8+x[i+1]*0.3, t*0.016+-0.8+y[i+1]*0.3, 0.0),
        cui3d::Vec3D(t*0.016+-0.8+x[i+1]*0.3, t*0.016+-0.8+y[i+1]*0.3, 0.3),
        cui3d::Vec3D(t*0.016+-0.8+x[i]*0.3, t*0.016+-0.8+y[i]*0.3, 0.3));
  }
  poly_v[0].triangles.emplace_back(
      cui3d::Vec3D(t*0.016+-0.8, t*0.016+-0.8, 0.0),
      cui3d::Vec3D(t*0.016+-0.8, t*0.016+-0.5, 0.0),
      cui3d::Vec3D(t*0.016+-0.5, t*0.016+-0.8, 0.0));
  poly_v[0].triangles.emplace_back(
      cui3d::Vec3D(t*0.016+-0.5, t*0.016+-0.5, 0.0),
      cui3d::Vec3D(t*0.016+-0.8, t*0.016+-0.5, 0.0),
      cui3d::Vec3D(t*0.016+-0.5, t*0.016+-0.8, 0.0));
  //bg.texture = {{'.'}};
  for (int i = 0; i < 4; ++i)
    poly_v[i+1].texture = {{(char)('+'+i)}};
  poly_v[0].texture = {{'@'}};
  cui3d::CuiImage img(h, w);
  //img = c.render(img, bg);
  img = c.render(img, poly_v);
  return img;
}

std::tuple<double, double, double> movie() {
  double img_gen_time = 0;
  double draw_time = 0;
  double render_time = 0;
  cui3d::Screen scr;
  for (int i = 0; i < 100; ++i) {
    double t = i;
    double process_time = 0;
    boost::timer tm;
    cui3d::CuiImage img = draw(scr.get_height(), scr.get_width(), t);
    img_gen_time += tm.elapsed();
    process_time += tm.elapsed();
    tm.restart();
    scr.draw(img);
    draw_time += tm.elapsed();
    process_time += tm.elapsed();
    tm.restart();
    scr.render();
    render_time += tm.elapsed();
    process_time += tm.elapsed();
    double rem = 1.0/60 - process_time;
    if (rem > 0) {
      timespec ts = {0, (long)(rem * 1000000000)};
      nanosleep(&ts, nullptr);
    }
  }
  return std::make_tuple(img_gen_time, draw_time, render_time);
}

int main() {
  double it, dt, rt;
  std::tie(it, dt, rt) = movie();
  std::cout << it << ' ' << dt << ' ' << rt << std::endl;
  return 0;
}
