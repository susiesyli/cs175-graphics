#ifndef SHAPE_H
#define SHAPE_H

#define GL_SILENCE_DEPRECATION

#include "construct.h"
#include "geometry.h"
#include <FL/gl.h>
#include <FL/glu.h>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

#define PI glm::pi<float>() // PI is now a constant for 3.14159....

using std::vector;

enum OBJ_TYPE {
  SHAPE_CUBE = 0,
  SHAPE_CYLINDER = 1,
  SHAPE_CONE = 2,
  SHAPE_SPHERE = 3,
  SHAPE_SPECIAL1 = 4,
  SHAPE_SPECIAL2 = 5,
  SHAPE_SPECIAL3 = 6,
  SHAPE_MESH = 7,
  SHAPE_SCENE = 8
};

class Shape {
public:
  int m_segmentsX;
  int m_segmentsY;

  Shape() {
    vertexList = NULL;
    vertexCount = 0;
    componentList = NULL;
    componentCount = 0;
    setSegments(10, 10);
  };

  ~Shape() {
    printf("deconstructed\n");
    deconstructVertices();
    if (componentList) {
      delete[] componentList;
    }
    componentList = NULL;
    componentCount = 0;
  };

  void setSegments(int segX, int segY) {
    if (m_segmentsX == segX && m_segmentsY == segY) {
      return;
    }
    printf("start constructing\n");
    m_segmentsX = segX;
    m_segmentsY = segY;
    deconstructVertices();
    constructVertices();
    printf("end constructing\n");
  }

  virtual OBJ_TYPE getType() = 0;

  void draw() { drawComponents(); };

  void flattenComponent() {
    flattenedComponent.clear();
    for (int comp_i = 0; comp_i < componentCount; comp_i++) {
      for (int i = 0; i < componentList[comp_i].size() - 1; i++) {
        const vector<int> &line1 = componentList[comp_i][i];
        const vector<int> &line2 = componentList[comp_i][i + 1];
        if (line1.size() == 1) {
          for (int m = 0; m < line2.size() - 1; m++) {
            flattenedComponent.push_back(line1[0]);
            flattenedComponent.push_back(line2[m]);
            flattenedComponent.push_back(line2[m + 1]);
          }
        } else if (line2.size() == 1) {
          for (int m = 0; m < line1.size() - 1; m++) {
            flattenedComponent.push_back(line2[0]);
            flattenedComponent.push_back(line1[m + 1]);
            flattenedComponent.push_back(line1[m]);
          }
        } else {
          int m = 0, n = 0;
          while (m < line1.size() - 1 && n < line2.size() - 1) {
            flattenedComponent.push_back(line1[m]);
            flattenedComponent.push_back(line2[n]);
            flattenedComponent.push_back(line2[n + 1]);

            n++;
            flattenedComponent.push_back(line1[m + 1]);
            flattenedComponent.push_back(line1[m]);
            flattenedComponent.push_back(line2[n]);
            m++;
          }
        }
      }
    }
  }

  void drawNormal() {
    glBegin(GL_LINES);
    for (int comp_i = 0; comp_i < componentCount; comp_i++) {
      for (int i = 0; i < componentList[comp_i].size(); i++) {
        for (int j = 0; j < componentList[comp_i][i].size(); j++) {
          vertex *v = vertexList[componentList[comp_i][i][j]];
          glVertex3f(v->x, v->y, v->z);
          glm::vec3 normal = 0.08f * glm::normalize(v->get_normal());
          glVertex3f(v->x + normal.x, v->y + normal.y, v->z + normal.z);
        }
      }
    }
    glEnd();
  };

protected:
  virtual void constructVertices(){};
  vertex **vertexList;
  int vertexCount;
  vector<vector<int>> *componentList;
  int componentCount;
  vector<int> flattenedComponent;

  void initializeComponents(int n_comp) {
    componentCount = n_comp;
    componentList = new vector<vector<int>>[componentCount];
    for (int i = 0; i < componentCount; i++) {
      componentList[i] = vector<vector<int>>();
    }
  }

  void normalizeNormal(float x, float y, float z) {
    normalizeNormal(glm::vec3(x, y, z));
  };

  void normalizeNormal(glm::vec3 v) {
    glm::vec3 tmpV = glm::normalize(v);
    glNormal3f(tmpV.x, tmpV.y, tmpV.z);
  };

  /// deconstruct all vectors in componentList and deallocate all vertices in
  /// vertexList
  void deconstructVertices() {
    for (int comp_i = 0; comp_i < componentCount; comp_i++) {
      for (int i = 0; i < componentList[comp_i].size(); i++) {
        componentList[comp_i][i].clear();
      }
      componentList[comp_i].clear();
    }
    for (int i = 0; i < vertexCount; i++) {
      delete vertexList[i];
    }
    printf("deconstruct %d vertices\n", vertexCount);
    if (vertexList) {
      delete[] vertexList;
    }
    vertexList = NULL;
    vertexCount = 0;
  }

  /// draw triangle and their normal given three points
  void drawTriangle(vertex *p1, vertex *p2, vertex *p3) {
    normalizeNormal(p1->get_normal());
    glVertex3f(p1->x, p1->y, p1->z);
    normalizeNormal(p2->get_normal());
    glVertex3f(p2->x, p2->y, p2->z);
    normalizeNormal(p3->get_normal());
    glVertex3f(p3->x, p3->y, p3->z);
  }

  void drawComponents() {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < flattenedComponent.size(); i += 3) {
      drawTriangle(vertexList[flattenedComponent[i]],
                   vertexList[flattenedComponent[i + 1]],
                   vertexList[flattenedComponent[i + 2]]);
    }
    glEnd();
  };

  /// add the allocated vertex v to the vertexList as well as
  /// the row-th row of the component-th component in componentList
  void addVertex(vertex *v, int component, int row) {
    vertexList[vertexCount] = v;
    componentList[component][row].push_back(vertexCount);
    vertexCount++;
  }
};

#endif
