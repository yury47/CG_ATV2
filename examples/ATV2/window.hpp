#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "camera.hpp"
#include "ground.hpp"
#include "cat.hpp"


// struct Vertex {
//   glm::vec3 position;

//   friend bool operator==(Vertex const &, Vertex const &) = default;
// };

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onUpdate() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  Camera m_camera;
  Ground m_ground;
  Cat m_cat;

  float m_dollySpeed{};
  float m_truckSpeed{};
  float m_panSpeed{};
  float m_catSpeed{};
  float m_catRotation{};
  float m_red{1.0f};
  float m_green{1.0f};
  float m_blue{1.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
};

#endif