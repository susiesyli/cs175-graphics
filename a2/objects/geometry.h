#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glm/glm.hpp>
class vertex {
public:
  float x, y, z;
  float nx, ny, nz;
  glm::vec3 get_position() { return glm::vec3(x, y, z); }
  glm::vec3 get_normal() { return glm::vec3(nx, ny, nz); }
  void set_normal(glm::vec3 norm) { set_normal(norm.x, norm.y, norm.z); }
  void set_normal(float x, float y, float z) {
    this->nx = x;
    this->ny = y;
    this->nz = z;
  }
  void set_position(glm::vec3 pos) { set_position(pos.x, pos.y, pos.z); }
  void set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  vertex *copy() {
    vertex *dup = new vertex();
    dup->set_position(get_position());
    dup->set_normal(get_normal());
    return dup;
  }
};

#endif
