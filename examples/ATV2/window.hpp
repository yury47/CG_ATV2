#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

struct Vertex {
  glm::vec3 position{};

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  float m_angle{};
  int m_verticesToDraw{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
  void standardize();

  float m_rotation{};
  float m_rotationX{0.0f}; // Rotação inicial no eixo X

  void update(float red, float green, float blue);
  float m_red{1.0f};
  float m_green{1.0f};
  float m_blue{1.0f};
};

#endif