#include "window.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "loadmodel.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "loadmodel.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Load model
  loadModelFromFile(assetsPath + "frog.obj");
  standardize();

  m_verticesToDraw = m_indices.size();

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attributes{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      auto const vx{attributes.vertices.at(startIndex + 0)};
      auto const vy{attributes.vertices.at(startIndex + 1)};
      auto const vz{attributes.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

      // If map doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void Window::standardize() {
  /*// Center to origin and normalize bounds to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{1.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }*/


  // Center to origin and normalize bounds to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{2.0f / glm::length(max - min)};

  // Define rotation matrices
  auto const rotationX{glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f))};
  auto const rotationY{glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f))};

  // Combine transformations: scaling, centering, and rotations
  auto const transformation{rotationY * rotationX * glm::scale(glm::mat4(1.0f), glm::vec3(scaling))};

  // Apply transformations
  for (auto &vertex : m_vertices) {
    glm::vec4 transformedPosition{vertex.position - center, 1.0f};
    transformedPosition = transformation * transformedPosition;
    vertex.position = glm::vec3(transformedPosition);
  }
}

void Window::onPaint() {
  // Animate angle by 15 degrees per second
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(45.0f) * deltaTime);

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);

  // Update uniform variable
  auto const angleLocation{abcg::glGetUniformLocation(m_program, "angle")};
  abcg::glUniform1f(angleLocation, m_angle);

  // Draw triangles
  abcg::glDrawElements(GL_TRIANGLES, m_verticesToDraw, GL_UNSIGNED_INT,
                       nullptr);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Window::update(float red, float green, float blue){

  m_red = red;
  m_green = green;
  m_blue = blue;
  

}

void Window::onPaintUI() {
  // Create a window for the other widgets
//    abcg::OpenGLWindow::onPaintUI();

  // Window begin
  ImGui::Begin("Cores");  
  if (ImGui::Button("+ Red", ImVec2(100, 50))) {
    m_red += 0.25f;
    if (m_red > 1.0f)
      m_red = 1.0f;
  }
  if (ImGui::Button("- Red", ImVec2(100, 50))) {
    m_red -= 0.25f;
    if (m_red < 0.0f)
      m_red = 0.0f;
  }
  if (ImGui::Button("+ Green", ImVec2(100, 50))) {
    m_green += 0.25f;
    if (m_green > 1.0f)
      m_green = 1.0f;
  }
  if (ImGui::Button("- Green", ImVec2(100, 50))) {
    m_green -= 0.25f;
    if (m_green < 0.0f)
      m_green = 0.0f;
  }
  if (ImGui::Button("+ Blue", ImVec2(100, 50))) {
    m_blue += 0.25f;
    if (m_blue > 1.0f)
      m_blue = 1.0f;
  }
  if (ImGui::Button("- Blue", ImVec2(100, 50))) {
    m_blue -= 0.25f;
    if (m_blue < 0.0f)
      m_blue = 0.0f;
  }

  update(m_red, m_green, m_blue);

  ImGui::End();

}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}