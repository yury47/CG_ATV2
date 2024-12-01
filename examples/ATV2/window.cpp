#include "window.hpp"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
      m_catSpeed = +3.5f;
      m_dollySpeed = -cosf(glm::radians(m_cat.m_rotation)) * m_catSpeed;
      m_truckSpeed = sinf(glm::radians(m_cat.m_rotation)) * m_catSpeed;
    }
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
      m_catSpeed = -3.5f;
      m_dollySpeed = -cosf(glm::radians(m_cat.m_rotation+180)) * (-m_catSpeed);
      m_truckSpeed = sinf(glm::radians(m_cat.m_rotation+180)) * (-m_catSpeed);
    }
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
      m_catRotation = 0.2f;
    }
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
      m_catRotation = -0.2f;
    }
    // if (event.key.keysym.sym == SDLK_q)
    //   m_truckSpeed = -1.0f;
    // if (event.key.keysym.sym == SDLK_e)
    //   m_truckSpeed = 1.0f;
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_catSpeed != 0) {
      m_dollySpeed = 0.0f;
      m_catSpeed = 0.0f;
      m_truckSpeed = 0.0f;
      m_catRotation = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_catSpeed != 0) {
      m_dollySpeed = 0.0f;
      m_catSpeed = 0.0f;
      m_truckSpeed = 0.0f;
      m_catRotation = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_catRotation != 0) {
      m_dollySpeed = 0.0f;
      m_catSpeed = 0.0f;
      m_truckSpeed = 0.0f;
      m_catRotation = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_catRotation != 0) {
      m_dollySpeed = 0.0f;
      m_catSpeed = 0.0f;
      m_truckSpeed = 0.0f;
      m_catRotation = 0.0f;
    }
    // if (event.key.keysym.sym == SDLK_q && m_truckSpeed < 0)
    //   m_truckSpeed = 0.0f;
    // if (event.key.keysym.sym == SDLK_e && m_truckSpeed > 0)
    //   m_truckSpeed = 0.0f;
  }
}

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_program);


  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");

  //m_cat.loadModelFromFile(assetsPath + "bunny.obj");
  
  m_cat.create(m_program);



  abcg::glBindVertexArray(0);

  
}



void Window::onPaint() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  

  m_cat.paint(m_viewportSize, m_camera, m_program);

  abcg::glBindVertexArray(0);

  // Draw ground
  m_ground.paint();



  abcg::glUseProgram(0);
}

void Window::onPaintUI() { 
  abcg::OpenGLWindow::onPaintUI();

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
  ImGui::End();
    

}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);

}

void Window::onDestroy() {
  m_ground.destroy();
  m_cat.destroy();



}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_cat.m_rotation += m_catRotation;
  m_cat.update(m_catSpeed, deltaTime, m_red, m_green, m_blue);

}