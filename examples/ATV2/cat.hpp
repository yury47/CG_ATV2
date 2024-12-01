#ifndef CAT_HPP_
#define CAT_HPP_

#include "abcgOpenGL.hpp"
#include "camera.hpp"


struct Vertex {
  glm::vec3 position;

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Cat {
public:
  void create(GLuint program);
  void loadModelFromFile(std::string_view path);
  void paint(glm::ivec2 m_viewportSize, Camera m_camera, GLuint program);
  void destroy();
  glm::vec3 m_translation{};
  float m_rotation{};
  float m_red{};
  float m_green{};
  float m_blue{};
  void update(float speed, float deltaTime, float red, float green, float blue);


private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  // Matrix to change from world space to camera space
  glm::mat4 m_viewMatrix;

  // Matrix to change from camera space to clip space
  glm::mat4 m_projMatrix;
};

#endif