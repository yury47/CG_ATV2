#include "window.hpp"

void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBall.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mousePress(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mouseRelease(mousePosition);
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj(assetsPath + "frog.obj");
  m_model.setupVAO(m_program);

  m_trianglesToDraw = m_model.getNumTriangles();
}

void Window::onUpdate() {
  m_modelMatrix = m_trackBall.getRotation();
  //auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  //auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_model.m_rotation += 10;
  m_model.update(m_red, m_green, m_blue);
  
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);

  // Set uniform variables for the current model
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  //abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White
  abcg::glUniform4f(colorLoc, m_red, m_green, m_blue, 1.0f); // White
  m_model.render(m_trianglesToDraw);
  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  // Create a window for the other widgets
  {
    {
      static std::size_t currentIndex{};
      if (currentIndex == 0) {
        auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                          gsl::narrow<float>(m_viewportSize.y)};
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
      } else {
        m_projMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }

  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};

    // Window begin
  ImGui::Begin("Cores");  
  if (ImGui::Button("+ Red", ImVec2(100, 50))) {
    m_red += 0.25f;
    if (m_red > 1.0f)
      m_red = 1.0f;
  }
  abcg::glUniform4f(colorLoc, m_red, m_green, m_blue, 1.0f);
  m_model.render(m_trianglesToDraw);
  if (ImGui::Button("- Red", ImVec2(100, 50))) {
    m_red -= 0.25f;
    if (m_red < 0.0f)
      m_red = 0.0f;
  }
  m_model.update(m_red, m_green, m_blue);
  if (ImGui::Button("+ Green", ImVec2(100, 50))) {
    m_green += 0.25f;
    if (m_green > 1.0f)
      m_green = 1.0f;
  }
  m_model.update(m_red, m_green, m_blue);
  if (ImGui::Button("- Green", ImVec2(100, 50))) {
    m_green -= 0.25f;
    if (m_green < 0.0f)
      m_green = 0.0f;
  }
  m_model.update(m_red, m_green, m_blue);
  if (ImGui::Button("+ Blue", ImVec2(100, 50))) {
    m_blue += 0.25f;
    if (m_blue > 1.0f)
      m_blue = 1.0f;
  }
  m_model.update(m_red, m_green, m_blue);
  if (ImGui::Button("- Blue", ImVec2(100, 50))) {
    m_blue -= 0.25f;
    if (m_blue < 0.0f)
      m_blue = 0.0f;
  }
  m_model.update(m_red, m_green, m_blue);
  ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_trackBall.resizeViewport(size);
}

void Window::onDestroy() {
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}