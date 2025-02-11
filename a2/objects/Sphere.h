#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "construct.h"

class Sphere : public Shape {
public:
  Sphere() {
    r = 0.5;
    y_start = 0.5;
    y_end = -0.5;
    initializeComponents(1);
    constructVertices();
  };

  OBJ_TYPE getType() { return SHAPE_SPHERE; }

  glm::vec3 computePosition(float u, float v) {
    return glm::vec3(r * glm::cos(v) * glm::cos(u), r * glm::sin(v),
                     -r * glm::cos(v) * glm::sin(u));
  };

  glm::vec3 computeNormal(float u, float v) {
    return glm::normalize(computePosition(u, v));
  }

protected:
  float r, y_start, y_end;
  void constructVertices() {
    for (int j = 0; j < m_segmentsY + 1; j++) {
      componentList[0].push_back(vector<int>());
    }

    vertexList = new vertex *[(m_segmentsY + 1) * (m_segmentsX + 1)];
    vector<vector<vertex *>> surface = constructRuledSurface(
        [this](float u, float v) -> glm::vec3 { // surface
          return this->computePosition(u, v);
        },
        (float[]){0, 2 * PI},             // x range
        (float[]){0.5f * PI, -0.5f * PI}, // y range
        m_segmentsX, m_segmentsY,
        [this](float u, float v) -> glm::vec3 { // normal
          return this->computeNormal(u, v);
        });

    for (int i = 0; i < surface.size(); i++) {
      vector<vertex *> row = surface[i];
      for (vertex *p : row) {
        addVertex(p, 0, i);
      }
    }

    printf("sphere: constructed %d vertices\n", vertexCount);
    
    flattenComponent();
  };
};

#endif
