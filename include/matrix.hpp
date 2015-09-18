#ifndef _HEADER_CUI3D_MATRIX_HPP_
#define _HEADER_CUI3D_MATRIX_HPP_
#include <cstddef>
#include <array>

namespace cui3d {

template <std::size_t R, std::size_t C>
struct Matrix {
 public:
  std::array<double, C> &operator[](const std::size_t row) { return data[row]; }
  const std::array<double, C> &operator[](const std::size_t row) const {
    return data[row];
  }
 private:
  std::array<std::array<double, C>, R> data;
};

template <std::size_t DIM>
struct Matrix<DIM, DIM> {
 public:
  Matrix<DIM, DIM>() {
    for (std::size_t i = 0; i < DIM; ++i)
      for (std::size_t j = 0; j < DIM; ++j)
        data[i][j] = (i == j ? 1 : 0);
  }
  std::array<double, DIM> &operator[](const std::size_t row) {
    return data[row];
  }
  const std::array<double, DIM> &operator[](const std::size_t row) const {
    return data[row];
  }
 private:
  std::array<std::array<double, DIM>, DIM> data;
};

template <std::size_t R, std::size_t C, std::size_t M>
Matrix<R, C> operator*(const Matrix<R, M> &lhs, const Matrix<M, C> &rhs) {
  Matrix<R, C> res;
  for (std::size_t i = 0; i < R; ++i)
    for (std::size_t j = 0; j < C; ++j)
      res[i][j] = 0;
  for (std::size_t i = 0; i < R; ++i)
    for (std::size_t k = 0; k < M; ++k)
      for (std::size_t j = 0; j < C; ++j)
        res[i][j] += lhs[i][k] * rhs[k][j];
  return res;
}

} // namespace cui3d
#endif
