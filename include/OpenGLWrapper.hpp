#pragma once

#include "Camera.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace GLColors {
constexpr glm::vec3 DARKGRAY{0.2f, 0.2f, 0.2f}, PINK{1.0f, 0.4f, 0.7f},
    ORANGE{1.0f, 0.5f, 0.0f}, DARKBLUE{0.0f, 0.0f, 0.5f},
    GREEN{0.0f, 0.5f, 0.0f}, GRAY{0.5f, 0.5f, 0.5f}, RED{1.0f, 0.0f, 0.0f};
}

class OpenGLWrapper {
private:
  GLFWwindow *window;
  int window_width;
  int window_height;
  std::string window_title;
  GLuint vao, vbo, shader_program;

  std::vector<Camera> cameras;
  int camera_id;

  void init_opengl();
  GLuint compile_shader(const char *source, GLenum type);
  void create_shader_program();
  glm::vec3 transform_scene(const glm::vec3 &model, float deg,
                            const glm::vec3 &axis, const glm::vec3 &translate);

  void set_camera_rotation(glm::vec3 rotation);
  void set_camera_position(glm::vec3 translation);
  glm::vec3 get_camera_rotation() const;
  glm::vec3 get_camera_position() const;

public:
  OpenGLWrapper(int width, int height, const std::string &title);
  ~OpenGLWrapper();

  void init();
  void draw_line(const glm::vec3 &start, const glm::vec3 &end,
                 const glm::vec3 &color);
  void draw_points(const std::vector<glm::vec3> &points,
                   const glm::vec3 &color);
  void draw_solid_shape(const std::vector<glm::vec3> &base_vertices,
                        float height, const glm::vec3 &color);

  void render_scene(const std::vector<std::vector<glm::vec3>> &contours,
                    const glm::vec3 &color);

  void update();
  void process_input();
  bool should_close() const;
};
