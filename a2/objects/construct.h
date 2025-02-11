#ifndef CONSTRUCT_H
#define CONSTRUCT_H
#include "geometry.h"
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

#define PI glm::pi<float>() // PI is now a constant for 3.14159....

std::vector<vertex *> constructRing(glm::vec3 center, float radius,
                                    glm::vec3 direction, int n_segments);

std::vector<std::vector<vertex *>>
constructRuledSurface(std::function<glm::vec3(float, float)> r,
                      float x_range[2], float y_range[2], int x_seg, int y_seg,
                      std::function<glm::vec3(float, float)> normal = NULL);

#endif
