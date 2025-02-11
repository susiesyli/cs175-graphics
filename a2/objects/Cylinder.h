#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include "construct.h"

class Cylinder : public Shape {
public:
  Cylinder() {
    r = 0.5;
    y_start = 0.5;
    y_end = -0.5;
    initializeComponents(3);
    constructVertices();
  };

  OBJ_TYPE getType() { return SHAPE_CYLINDER; }

  glm::vec3 computePosition(float u, float v) {
    return glm::vec3(r * glm::cos(u), 0.0f, r * glm::sin(u)) +
           v * glm::vec3(0.0f, 1.0f, 0.0f);
  };

protected:
  float r, y_start, y_end;
  void constructVertices() {
    for (int j = 0; j < m_segmentsY + 1; j++) {
      componentList[0].push_back(vector<int>());
    }
    componentList[1].push_back(vector<int>());
    componentList[1].push_back(vector<int>());
    componentList[2].push_back(vector<int>());
    componentList[2].push_back(vector<int>());

    vertexList = new vertex
        *[(m_segmentsY + 1) * (m_segmentsX + 1) + 2 * (m_segmentsX + 1 + 1)];
    vector<vector<vertex *>> cylinder_side = constructRuledSurface(
        [this](float u, float v) -> glm::vec3 {
          return this->computePosition(u, v);
        },
        (float[]){2 * PI, 0},      // x range
        (float[]){y_start, y_end}, // y range
        m_segmentsX, m_segmentsY);

    // add top cap
    for (vertex *p : cylinder_side[0]) {
      vertex *p_dup = p->copy();
      p_dup->set_normal(0.0f, 1.0f, 0.0f);
      addVertex(p_dup, 1, 1);
    }
    vertex *top = new vertex();
    top->set_position(0.0f, y_start, 0.0f);
    top->set_normal(0.0f, 1.0f, 0.0f);
    addVertex(top, 1, 0);

    // add bottom cap
    for (vertex *p : cylinder_side[m_segmentsY]) {
      vertex *p_dup = p->copy();
      p_dup->set_normal(0.0f, -1.0f, 0.0f);
      addVertex(p_dup, 2, 0);
    }
    vertex *bot = new vertex();
    bot->set_position(0.0f, y_end, 0.0f);
    bot->set_normal(0.0f, -1.0f, 0.0f);
    addVertex(bot, 2, 1);

    for (int i = 0; i < cylinder_side.size(); i++) {
      vector<vertex *> row = cylinder_side[i];
      for (vertex *p : row) {
        p->set_normal(p->x, 0, p->z);
        addVertex(p, 0, i);
      }
    }

    printf("cylinder: constructed %d vertices\n", vertexCount);
    flattenComponent();
  };
};

#endif
