#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

class Cube : public Shape {
public:
	Cube() {};
	~Cube() {};

	OBJ_TYPE getType() {
		return SHAPE_CUBE;
	}

    double intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix) {
        glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
        
        // Convert to object space
        glm::vec4 objP = inverseMatrix * glm::vec4(eyePointP, 1.0f);
        glm::vec4 objRay = inverseMatrix * glm::vec4(rayV, 0.0f);
        
        glm::vec3 P = glm::vec3(objP);
        glm::vec3 d = glm::vec3(objRay);
        float t_min = -std::numeric_limits<float>::infinity();
        float t_max = std::numeric_limits<float>::infinity();

        // Check intersection with X planes
        if (std::abs(d.x) < 1e-6) {
            // if origin is not within 0.5f then it won't intersect
            if (P.x < -0.5f || P.x > 0.5f) return -1;
        } else {
            float t1 = (-0.5f - P.x) / d.x;
            float t2 = (0.5f - P.x) / d.x;
            if (t1 > t2) std::swap(t1, t2);
            
            t_min = std::max(t_min, t1);
            t_max = std::min(t_max, t2);
            
            if (t_min > t_max) return -1;
        }

        // Check intersection with Y planes
        if (std::abs(d.y) < 1e-6) {
            if (P.y < -0.5f || P.y > 0.5f) return -1;
        } else {
            float t1 = (-0.5f - P.y) / d.y;
            float t2 = (0.5f - P.y) / d.y;
            if (t1 > t2) std::swap(t1, t2);
            
            t_min = std::max(t_min, t1);
            t_max = std::min(t_max, t2);
            
            if (t_min > t_max) return -1;
        }

        // Check intersection with Z planes
        if (std::abs(d.z) < 1e-6) {
            if (P.z < -0.5f || P.z > 0.5f) return -1;
        } else {
            float t1 = (-0.5f - P.z) / d.z;
            float t2 = (0.5f - P.z) / d.z;
            if (t1 > t2) std::swap(t1, t2);
            
            t_min = std::max(t_min, t1);
            t_max = std::min(t_max, t2);
            
            if (t_min > t_max) return -1;
        }
        if (t_min > 0) return t_min;
        if (t_max > 0) return t_max;
        return -1;
    }

	float draw(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix) {
        return intersect(eyePoint, ray, transformMatrix);
	};

    glm::vec2 getUVCoordinates(glm::vec3 ray) {
        glm::vec2 uv;

        if (std::abs(ray.x) >= std::abs(ray.y) && std::abs(ray.x) >= std::abs(ray.z)) { 
            // x-dominant face
            if (ray.x > 0) { // pos x face
                uv.x = -ray.z / ray.x;  // flip z for left-to-right
                uv.y = ray.y / ray.x;
            } else { // neg x face
                uv.x = ray.z / ray.x;   
                uv.y = ray.y / ray.x;
            }
        }
        else if (std::abs(ray.y) >= std::abs(ray.x) && std::abs(ray.y) >= std::abs(ray.z)) { 
            // y-dominant face
            if (ray.y > 0) { // pos y face (top)
                uv.x = ray.x / ray.y;   
                uv.y = ray.z / ray.y;   
            } else { // neg y face 
                uv.x = ray.x / ray.y;
                uv.y = ray.z / ray.y;
            }
        }
        else { 
            // z-dominant face
            if (ray.z > 0) { // pos z face
                uv.x = ray.x / ray.z;
                uv.y = ray.y / ray.z;
            } else { // neg z face
                uv.x = -ray.x / ray.z;  // flip z for left-to-right 
                uv.y = ray.y / ray.z;
            }
        }

        uv = (uv + glm::vec2(1.0f)) * 0.5f;
        return uv;
    }

    glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 worldSpacePos, glm::mat4 transformMatrix, double t) {
        glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
        // Convert eye point to object space - should use w=1.0 for points
        glm::vec4 objP = inverseMatrix * glm::vec4(eyePoint, 1.0f);
        // Convert ray to object space - should use w=0.0 for vectors
        glm::vec4 intersectPoint = inverseMatrix * glm::vec4(worldSpacePos, 1.0f);

        // for a cube, we have 3 cases: x-plane, y-plane, or z-plane intersection normal  
        // we can determine which face the ray intersects with by 
        // finding the largest intersect component: x, y or z 
        glm::vec3 absIntersect = glm::abs(intersectPoint);
        glm::vec3 normalObj;

        // case 1: x-plane intersection: x >= y and x >= z 
        if (absIntersect.x >= absIntersect.y && absIntersect.x >= absIntersect.z)
            normalObj = glm::vec3(glm::sign(intersectPoint.x), 0, 0);
        // case 2: y-plane intersection: y >= z 
        else if (absIntersect.y >= absIntersect.z)
            normalObj = glm::vec3(0, glm::sign(intersectPoint.y), 0);
        // case 3: z-plane intersection
        else
            normalObj = glm::vec3(0, 0, glm::sign(intersectPoint.z));

        // Transform normal to world coordinates using the normal transform matrix
        // For normals, we need to use the transpose of the inverse of the 3x3 part
        glm::mat3 normalMatrix = glm::transpose(glm::mat3(inverseMatrix));
        glm::vec3 worldNormal = glm::normalize(normalMatrix * normalObj);

        return worldNormal;
    }

private:
};

#endif




