#include "construct.h"

using std::vector;

/// construct a rotation vector based on theta and the vector that will be
/// rotated upon. using Rodrigues rotation formula
/// https://math.stackexchange.com/questions/142821/matrix-for-rotation-around-a-vector
glm::mat3 getRotationVec(float theta, glm::vec3 axis) {
  glm::mat3 rot(1.0f);
  glm::mat3 w(0.0f);
  w[0][1] = axis.z;
  w[0][2] = -axis.y;
  w[1][0] = -axis.z;
  w[1][2] = axis.x;
  w[2][0] = axis.y;
  w[2][1] = -axis.x;
  rot = rot + glm::sin(theta) * w +
        (2 * (float)(glm::pow(glm::sin(theta / 2), 2))) * w * w;
  return rot;
}

/// construct a ring of n_segment+1 points in 3D space, at center with radius.
/// direction specifies the normal vector that the ring will be facing towards.
vector<vertex *> constructRing(glm::vec3 center, float radius,
                               glm::vec3 direction, int n_segments) {
  glm::vec3 normal = glm::normalize(direction);
  glm::vec3 start;
  if (normal.x != 0 || normal.y != 0) {
    start.x = -normal.y;
    start.y = normal.x;
    start.z = 0;
  } else {
    start.x = 0;
    start.y = -normal.z;
    start.z = normal.y;
  }

  start = glm::normalize(start) * radius;
  float angle = 0;
  float angle_sep = 2 * PI / ((float)n_segments);
  vector<vertex *> res = vector<vertex *>();
  for (int i = 0; i < n_segments + 1; i++) {
    glm::mat3 rotvec = getRotationVec(angle, normal);
    glm::vec3 rotated = start * rotvec + center;
    angle += angle_sep;
    vertex *v = new vertex();
    v->set_position(rotated);
    res.push_back(v);
  }

  return res;
}

vector<vector<vertex *>>
constructRuledSurface(std::function<glm::vec3(float, float)> r,
                      float x_range[2], float y_range[2], int x_seg, int y_seg,
                      std::function<glm::vec3(float, float)> normal) {
  float u = x_range[0];
  float u_f = x_range[1];
  float u_sep = (u_f - u) / (float)(x_seg);
  float v = y_range[0];
  float v_f = y_range[1];
  float v_sep = (v_f - v) / (float)y_seg;
  vector<vector<vertex *>> res = vector<vector<vertex *>>();
  v = y_range[0];
  for (int yi = 0; yi < y_seg + 1; yi++) {
    u = x_range[0];
    res.push_back(vector<vertex *>());
    for (int xi = 0; xi < x_seg + 1; xi++) {
      glm::vec3 pos = r(u, v);
      // printf("row %d col %d: u=%f, v=%f || pos=(%f, %f, %f)\n", yi, xi, u, v,
      // pos.x, pos.y, pos.z);
      vertex *p = new vertex();
      p->set_position(pos);
      if (normal) {
        p->set_normal(normal(u, v));
      }
      res[yi].push_back(p);
      u += u_sep;
    }
    v += v_sep;
  }
  return res;
}
