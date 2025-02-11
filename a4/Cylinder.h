#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"

class Cylinder : public Shape {
public:
	Cylinder() {};
	~Cylinder() {};

	OBJ_TYPE getType() {
		return SHAPE_CYLINDER;
	} 

    glm::vec2 getUVCoordinates(glm::vec3 point) {
        // Check if we're on the caps first
        const float EPSILON = 1e-4f;
        if (std::abs(point.y - 0.5f) < EPSILON || std::abs(point.y + 0.5f) < EPSILON) {
            // On top or bottom cap - use planar mapping
            return glm::vec2(
                (-point.x  / point.y+ 1.0f) * 0.5f,
                (point.z / point.y+ 1.0f) * 0.5f
            );
        }

        // Map to UV coordinates
        float theta = atan2(-point.z, point.x);
        float u = (theta + M_PI) / (2.0f * M_PI);
        float v = point.y + 0.5f;                  // map [-0.5,0.5] to [0,1]
        
        return glm::vec2(u, v);
    }

    float intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix) {
        glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
        glm::vec4 objP = inverseMatrix * glm::vec4(eyePointP, 1.0f);
        glm::vec4 objRay = inverseMatrix * glm::vec4(rayV, 0.0f);
        
        glm::vec3 P = glm::vec3(objP);
        glm::vec3 d = glm::vec3(objRay);
        
        float radius = 0.5f;
        
        float a = d.x * d.x + d.z * d.z;
        float b = 2.0f * (P.x * d.x + P.z * d.z);
        float c = P.x * P.x + P.z * P.z - radius * radius;
        
        float discriminant = b * b - 4.0f * a * c;
        
        float t_body = std::numeric_limits<float>::infinity();
        if (discriminant >= 0) {
            float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
            float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
            
            // changed to negate y coordinates for intersection checks
            float y1 = -(P.y + t1 * d.y);  
            float y2 = -(P.y + t2 * d.y);   
            
            if (t1 > 0 && y1 >= -0.5f && y1 <= 0.5f) t_body = t1;
            else if (t2 > 0 && y2 >= -0.5f && y2 <= 0.5f) t_body = t2;
        }
        
        float t_caps = std::numeric_limits<float>::infinity();
        if (std::abs(d.y) > 1e-6) {
            float t1 = (0.5f - P.y) / d.y;   
            float t2 = (-0.5f - P.y) / d.y; 
            
            // check cap radius range 
            glm::vec3 p1 = P + t1 * d;
            glm::vec3 p2 = P + t2 * d;
            
            if (t1 > 0 && p1.x * p1.x + p1.z * p1.z <= radius * radius) t_caps = t1;
            if (t2 > 0 && p2.x * p2.x + p2.z * p2.z <= radius * radius) 
                t_caps = (t_caps == std::numeric_limits<float>::infinity()) ? t2 : std::min(t_caps, t2);
        }
        float t = std::min(t_body, t_caps);
        return t == std::numeric_limits<float>::infinity() ? -1 : t;
    }

	float draw(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix) {
        return intersect(eyePoint, ray, transformMatrix);
	};

	//glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix, double t) {
 //       glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
 //       // convert eye point to object space - should use w=1.0 for points
 //       glm::vec4 objP = inverseMatrix * glm::vec4(eyePoint, 1.0f);
 //       // convert ray to object space - should use w=0.0 for vectors
 //       glm::vec4 objRay = inverseMatrix * glm::vec4(ray, 0.0f);
 //       // calculate intersection point in object coordinates
 //       glm::vec3 objRayNorm = glm::normalize(glm::vec3(objRay));
 //       glm::vec3 intersectPoint = glm::vec3(objP) + float(t) * objRayNorm;

 //       // for a cylinder, we need to check 3 cases: top cap, bottom cap, body
 //       glm::vec3 normalObj;
 //       if (std::abs(intersectPoint.y - 0.5f) < 0.0001f) { // top cap 
 //           normalObj = glm::vec3(0, 1, 0);
 //       } else if (std::abs(intersectPoint.y + 0.5f) < 0.0001f) { // bottom cap 
 //           normalObj = glm::vec3(0, -1, 0);
 //       } else { // cylinder body
 //           normalObj = glm::normalize(glm::vec3(intersectPoint.x, 0, intersectPoint.z));
 //       }
 //       // Transform normal to world coordinates using the normal transform matrix
 //       // For normals, we need to use the transpose of the inverse of the 3x3 part
 //       glm::mat3 normalMatrix = glm::transpose(glm::mat3(inverseMatrix));
 //       glm::vec3 worldNormal = glm::normalize(normalMatrix * normalObj);

 //       return worldNormal;
	//};

    glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 worldSpacePos, glm::mat4 transformMatrix, double t) {
        glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
        // Convert eye point to object space - should use w=1.0 for points
        glm::vec4 objP = inverseMatrix * glm::vec4(eyePoint, 1.0f);
        // Convert ray to object space - should use w=0.0 for vectors
        glm::vec4 intersectPoint = inverseMatrix * glm::vec4(worldSpacePos, 1.0f);

        // For a sphere, the normal is just the normalized point (since it's centered at origin)
        //glm::vec3 normalObj = glm::normalize(intersectPoint);

        // for a cylinder, we need to check 3 cases: top cap, bottom cap, body
        glm::vec3 normalObj;
        if (std::abs(intersectPoint.y - 0.5f) < 0.0001f) { // top cap 
            normalObj = glm::vec3(0, 1, 0);
        }
        else if (std::abs(intersectPoint.y + 0.5f) < 0.0001f) { // bottom cap 
            normalObj = glm::vec3(0, -1, 0);
        }
        else { // cylinder body
            normalObj = glm::normalize(glm::vec3(intersectPoint.x, 0, intersectPoint.z));
        }

        // Transform normal to world coordinates using the normal transform matrix
        // For normals, we need to use the transpose of the inverse of the 3x3 part
        glm::mat3 normalMatrix = glm::transpose(glm::mat3(inverseMatrix));
        glm::vec3 worldNormal = glm::normalize(normalMatrix * normalObj);

        return worldNormal;
    }


private:
};

#endif