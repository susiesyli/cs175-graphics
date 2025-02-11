
#ifndef SPIRAL_H
#define SPIRAL_H
#include "Shape.h"
#include "construct.h"
#include "geometry.h"
#include <OpenGL/OpenGL.h>

class Spiral : public Shape {

public:
  Spiral() {
    a = 0.002;
    b = 0.17;
    theta_start = 3 * PI;
    theta_stop = 12 * PI;
    initializeComponents(2);
    constructVertices();
  };

  OBJ_TYPE getType() { return SHAPE_SPECIAL1; }

protected:
  float a;
  float b;
  float theta_start;
  float theta_stop;

  /// compute the radius on the curve given theta
  float spiralRadius(float theta) {
    float radius = a * glm::exp(b * theta);
    return radius;
  }

  /// compute the radius of the ring to be drawn
  float getRealRadius(float theta) {
    return (spiralRadius(theta) - spiralRadius(theta - 2 * PI)) / 2;
  }

  /// compute the center of ring to be drawn (located in the middle point
  /// between theta and theta-2PI)
  glm::vec3 getCenter(float theta) {
    float radius = spiralRadius(theta);
    float x_center = glm::cos(theta) * radius;
    float y_center = glm::sin(theta) * radius;
    float prev_radius = spiralRadius(theta - 2 * PI);
    float x_center_prev = glm::cos(theta - 2 * PI) * prev_radius;
    float y_center_prev = glm::sin(theta - 2 * PI) * prev_radius;
    return glm::vec3(x_center + x_center_prev, y_center + y_center_prev, 0) /
           2.0f;
  }

  /// construct all vertices in vertexList, and construct the componentList
  /// which references the vertex indices
  void constructVertices() {
    for (int j = 0; j < m_segmentsY + 1; j++) {
      componentList[0].push_back(vector<int>());
    }
    componentList[1].push_back(vector<int>());
    componentList[1].push_back(vector<int>());

    vertexList = new vertex
        *[(m_segmentsY + 1) * (m_segmentsX + 1) + (m_segmentsX + 1) + 1];

    float theta = theta_start;
    float theta_step = (theta_stop - theta_start) / m_segmentsY;

    float radius;
    for (int j = 0; j < m_segmentsY + 1; j++) {
      radius = getRealRadius(theta);
      componentList[0][j] = vector<int>();
      glm::vec3 center = getCenter(theta);
      glm::vec3 normal = glm::vec3(-center.y, center.x, 0);
      auto ring = constructRing(glm::vec3(center.x, center.y, 0), radius,
                                normal, m_segmentsX);
      for (vertex *p : ring) {
        p->set_normal(p->get_position() - center);
      }
      theta += theta_step;
      for (int v_c = 0; v_c < ring.size(); v_c++) {
        addVertex(ring[v_c], 0, j);
      }
      if (j == m_segmentsY) { // the last iteration
        for (int v_c = 0; v_c < ring.size(); v_c++) {

          vertex *v_dup = ring[v_c]->copy();
          v_dup->set_normal(0.0f, 1.0f, 0.0f);
          addVertex(v_dup, 1, 0);
        }
        vertex *vf = new vertex();
        vf->set_position(center);
        vf->set_normal(0.0f, 1.0f, 0.0f);
        addVertex(vf, 1, 1);
      }
    }

    printf("spiral: constructed %d vertices\n", vertexCount);
  }
};

#endif
