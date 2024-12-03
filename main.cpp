#include "OpenGLWrapper.hpp"
#include <glm/glm.hpp>

int main() {
  OpenGLWrapper opengl(1920, 1080, "rlb");

  opengl.init();

  std::vector<std::vector<glm::vec3>> placeholder_contour = {
      {glm::vec3(-0.3f, 0.7f, 0.2f), glm::vec3(0.6f, 0.4f, -0.1f),
       glm::vec3(0.8f, -0.5f, 0.3f), glm::vec3(0.1f, -0.8f, -0.2f),
       glm::vec3(-0.7f, -0.3f, 0.1f), glm::vec3(-0.5f, 0.2f, -0.3f)},
      {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f),
       glm::vec3(0.5f, -0.5f, 0.0f)}};

  while (!opengl.should_close()) {

    opengl.process_input();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    opengl.render_scene(placeholder_contour, GLColors::GRAY);

    opengl.update();
  }

  return 0;
}
