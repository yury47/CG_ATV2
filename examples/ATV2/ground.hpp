#ifndef GROUND_HPP_
#define GROUND_HPP_

#include "abcgOpenGL.hpp"
#include <random>

class Ground {
public:
  void create(GLuint program);
  void paint();
  void destroy();

private:
  GLuint m_VAO{};
  GLuint m_VBO{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomColor{0, 1};
};

#endif