#include "OpenGLWrapper.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

OpenGLWrapper::OpenGLWrapper(int width, int height, const std::string &title)
    : window_width(width), window_height(height), window_title(title),
      camera_rotation(0.0f, 0.0f, 0.0f), camera_translation(0.0f, 0.0f, 0.0f) {
  window = nullptr;
  vao = 0;
  vbo = 0;
  shader_program = 0;
}
OpenGLWrapper::~OpenGLWrapper() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void OpenGLWrapper::init_opengl() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(window_width, window_height, window_title.c_str(),
                            NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window!" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW!" << std::endl;
    exit(EXIT_FAILURE);
  }
}

GLuint OpenGLWrapper::compile_shader(const char *source, GLenum type) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    exit(EXIT_FAILURE);
  }

  return shader;
}

void OpenGLWrapper::create_shader_program() {
  const char *vertex_shader_source = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        out vec3 vertexColor;
        uniform mat4 model;
        void main() {
            gl_Position = model * vec4(aPos, 1.0);
            vertexColor = aColor;
        })";

  const char *fragment_shader_source = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vertexColor, 1.0);
        })";

  GLuint vertex_shader = compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
  GLuint fragment_shader =
      compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  GLint success;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    std::cerr << "Program linking failed: " << infoLog << std::endl;
    exit(EXIT_FAILURE);
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void OpenGLWrapper::init() {
  init_opengl();
  create_shader_program();
}

void OpenGLWrapper::draw_line(const glm::vec3 &start, const glm::vec3 &end,
                              const glm::vec3 &color) {

  GLfloat vertices[] = {start.x, start.y, start.z, color.r, color.g, color.b,
                        end.x,   end.y,   end.z,   color.r, color.g, color.b};

  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glUseProgram(shader_program);

  glm::mat4 model = glm::mat4(1.0f);
  GLint modelLoc = glGetUniformLocation(shader_program, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  glLineWidth(2.0f);

  glDrawArrays(GL_LINES, 0, 2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void OpenGLWrapper::draw_points(const std::vector<glm::vec3> &points,
                                const glm::vec3 &color) {
  if (points.empty()) {
    std::cerr << "Warning: Attempted to draw empty points vector" << std::endl;
    return;
  }

  std::vector<GLfloat> vertices;
  for (const auto &point : points) {
    vertices.push_back(point.x);
    vertices.push_back(point.y);
    vertices.push_back(point.z);

    vertices.push_back(color.r);
    vertices.push_back(color.g);
    vertices.push_back(color.b);
  }

  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glUseProgram(shader_program);

  glm::mat4 model = glm::mat4(1.0f);
  GLint modelLoc = glGetUniformLocation(shader_program, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  glPointSize(5.0f);

  glDrawArrays(GL_POINTS, 0, points.size());

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void OpenGLWrapper::draw_solid_shape(
    const std::vector<glm::vec3> &base_vertices, float height,
    const glm::vec3 &color) {
  if (base_vertices.size() < 3) {
    std::cerr << "Warning: Not enough vertices to draw a solid shape"
              << std::endl;
    return;
  }

  std::vector<GLfloat> data;
  size_t num_vertices = base_vertices.size();

  for (const auto &vertex : base_vertices) {
    data.push_back(vertex.x);
    data.push_back(vertex.y);
    data.push_back(vertex.z);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);
  }

  for (const auto &vertex : base_vertices) {
    data.push_back(vertex.x);
    data.push_back(vertex.y);
    data.push_back(vertex.z + height);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);
  }

  for (size_t i = 0; i < num_vertices; ++i) {
    size_t next = (i + 1) % num_vertices;

    data.push_back(base_vertices[i].x);
    data.push_back(base_vertices[i].y);
    data.push_back(base_vertices[i].z);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);

    data.push_back(base_vertices[next].x);
    data.push_back(base_vertices[next].y);
    data.push_back(base_vertices[next].z);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);

    data.push_back(base_vertices[i].x);
    data.push_back(base_vertices[i].y);
    data.push_back(base_vertices[i].z + height);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);

    data.push_back(base_vertices[next].x);
    data.push_back(base_vertices[next].y);
    data.push_back(base_vertices[next].z);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);

    data.push_back(base_vertices[next].x);
    data.push_back(base_vertices[next].y);
    data.push_back(base_vertices[next].z + height);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);

    data.push_back(base_vertices[i].x);
    data.push_back(base_vertices[i].y);
    data.push_back(base_vertices[i].z + height);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);
  }

  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glUseProgram(shader_program);

  glm::mat4 model = glm::mat4(1.0f);
  GLint modelLoc = glGetUniformLocation(shader_program, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  glEnable(GL_DEPTH_TEST);

  glDrawArrays(GL_TRIANGLES, 0, data.size() / 6);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void OpenGLWrapper::render_scene(
    const std::vector<std::vector<glm::vec3>> &contours,
    const glm::vec3 &color) {
  int i = 0;
  for (const auto &points : contours) {
    std::vector<glm::vec3> transformedPoints(points.size());
    std::transform(points.begin(), points.end(), transformedPoints.begin(),
                   [this](const glm::vec3 &point) {
                     return rotate(point, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f));
                   });
    draw_solid_shape(transformedPoints, 4.0f,
                     i++ ? GLColors::GRAY : GLColors::PINK);
  }
}

void OpenGLWrapper::update() {
  glfwSwapBuffers(window);
  glfwPollEvents();
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

glm::vec3 OpenGLWrapper::rotate(const glm::vec3 &model, float deg,
                                const glm::vec3 &axis,
                                const glm::vec3 &translate) {
  glm::mat4 model_ortho = glm::mat4(1.0f);
  model_ortho = glm::translate(model_ortho, camera_translation);
  glm::mat4 transformationMatrix =
      glm::rotate(model_ortho, glm::radians(deg), axis);
  glm::vec4 transformed = model_ortho * glm::vec4(model, 1.0f);
  return glm::vec3(transformed);
}

bool OpenGLWrapper::should_close() const {
  return glfwWindowShouldClose(window);
}

void OpenGLWrapper::set_camera_rotation(glm::vec3 rotation) {
  camera_rotation = rotation;
}

void OpenGLWrapper::set_camera_translation(glm::vec3 translation) {
  camera_translation = translation;
}

glm::vec3 OpenGLWrapper::get_camera_rotation() const { return camera_rotation; }

glm::vec3 OpenGLWrapper::get_camera_translation() const {
  return camera_translation;
}

void OpenGLWrapper::process_input() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
    return;
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    set_camera_translation(get_camera_translation() +
                           glm::vec3(0.0f, 0.0f, 0.1f));
    return;
  }

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    set_camera_translation(get_camera_translation() +
                           glm::vec3(0.0f, 0.0f, -0.1f));
    return;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    set_camera_translation(get_camera_translation() +
                           glm::vec3(-0.1f, 0.0f, 0.0f));
    return;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    set_camera_translation(get_camera_translation() +
                           glm::vec3(0.1f, 0.0f, 0.0f));
    return;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    set_camera_translation(get_camera_translation() +
                           glm::vec3(0.0f, -0.1f, 0.0f));
    return;
  }
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
    set_camera_translation(get_camera_translation() +
                           glm::vec3(0.0f, 0.1f, 0.0f));
    return;
  }
}
