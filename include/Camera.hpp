#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
private:
  glm::vec3 position;
  glm::vec3 rotation;
  // TODO: Add rotation angle
  // TODO: Add camera type

public:
  Camera(glm::vec3 position, glm::vec3 rotation)
      : position(position), rotation(rotation) {}

  void set_position(const glm::vec3 &new_position) { position = new_position; }

  void set_rotation(const glm::vec3 &new_rotation) { rotation = new_rotation; }

  glm::vec3 get_position() const { return position; }

  glm::vec3 get_rotation() const { return rotation; }
};
