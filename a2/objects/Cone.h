#ifndef CONE_H
#define CONE_H

#include "Shape.h"
#include "construct.h"

class Cone : public Shape {
public:
  Cone() {
    r = 0.5;
    y_start = 0.5;
    y_end = -0.5;
    initializeComponents(2);
    constructVertices();
  };

  OBJ_TYPE getType() { return SHAPE_CONE; }

  glm::vec3 computePosition(float u, float v) {
    float m = (0.5 - v) / 2;
    return glm::vec3(m * glm::cos(u), v, -m * glm::sin(u));
  };

  glm::vec3 computeNormal(float u, float v) {
    if (glm::abs(v - 0.5) < 0.001) {
      return glm::vec3(0.0f, 0.0f, 0.0f);
    }
    float normal_s = 0.5;
    return (glm::vec3((1 / normal_s) * glm::cos(u), 1.0f,
                      -(1 / normal_s) * glm::sin(u)));
  }

protected:
  float r, y_start, y_end;
  void constructVertices() {
    for (int j = 0; j < m_segmentsY + 1; j++) {
      componentList[0].push_back(vector<int>());
    }

    componentList[1].push_back(vector<int>());
    componentList[1].push_back(vector<int>());

    vertexList = new vertex
        *[(m_segmentsY + 1) * (m_segmentsX + 1) + (m_segmentsX + 1 + 1)];
    vector<vector<vertex *>> cone_side = constructRuledSurface(
        [this](float u, float v) -> glm::vec3 {
          return this->computePosition(u, v);
        },
        (float[]){0.0f, 2 * PI},   // x range
        (float[]){y_start, y_end}, // y range
        m_segmentsX, m_segmentsY,
        [this](float u, float v) -> glm::vec3 {
          return this->computeNormal(u, v);
        });

    // add bottom cap
    for (vertex *p : cone_side[cone_side.size() - 1]) {
      vertex *p_dup = p->copy();
      p_dup->set_normal(0.0f, -1.0f, 0.0f);
      addVertex(p_dup, 1, 0);
    }
    vertex *bot = new vertex();
    bot->set_position(0.0f, y_end, 0.0f);
    bot->set_normal(0.0f, -1.0f, 0.0f);
    addVertex(bot, 1, 1);

    for (int i = 0; i < cone_side.size(); i++) {
      vector<vertex *> row = cone_side[i];
      for (vertex *p : row) {
        addVertex(p, 0, i);
      }
    }

    printf("cone: constructed %d vertices\n", vertexCount);
    flattenComponent();
  };
};

#endif
