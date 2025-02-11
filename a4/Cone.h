#ifndef CONE_H
#define CONE_H

#include "Shape.h"

class Cone : public Shape {
public:
	Cone() {};
	~Cone() {};

	OBJ_TYPE getType() {
		return SHAPE_CONE;
	}

    float intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix) {
        glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
        
        // Convert to object space
        glm::vec4 objP = inverseMatrix * glm::vec4(eyePointP, 1.0f);
        glm::vec4 objRay = inverseMatrix * glm::vec4(rayV, 0.0f);
        
        glm::vec3 P = glm::vec3(objP);
        glm::vec3 d = glm::vec3(objRay);
        
        // Define cone parameters
        float height = 1.0f;            // Height is 1 unit
        float apex_y = 0.5f;            // Apex is at y = 0.5
        float r = 0.5f;                 // Base radius
        float r_squared = r * r;
        float h_squared = height * height;
        
        float a = d.x * d.x + d.z * d.z - (d.y * d.y * r_squared / h_squared);
        float b = 2.0f * (P.x * d.x + P.z * d.z + 
                         (apex_y - P.y) * d.y * r_squared / h_squared);
        float c = P.x * P.x + P.z * P.z - 
                 ((apex_y - P.y) * (apex_y - P.y) * r_squared / h_squared);
        
        float discriminant = b * b - 4.0f * a * c;
        
        float t_body = std::numeric_limits<float>::infinity();
        if (discriminant >= 0) {
            float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
            float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
            
            // Check if intersection rays are within cone height
            float y1 = P.y + t1 * d.y;
            float y2 = P.y + t2 * d.y;
            
            if (t1 > 0 && y1 >= -0.5f && y1 <= 0.5f) t_body = t1;
            else if (t2 > 0 && y2 >= -0.5f && y2 <= 0.5f) t_body = t2;
        }
        
        // Check base cap (y = -0.5 plane)
        float t_cap = std::numeric_limits<float>::infinity();
        if (std::abs(d.y) > 1e-6) {
            float t = (-0.5f - P.y) / d.y;
            
            // Check if intersection ray is within base radius
            glm::vec3 p = P + t * d;
            if (t > 0 && (p.x * p.x + p.z * p.z) <= r_squared) {
                t_cap = t;
            }
        }
        // Return nearest intersection
        float t = std::min(t_body, t_cap);
        return t == std::numeric_limits<float>::infinity() ? -1 : t;
    }
    
	float draw(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix) {
		return intersect(eyePoint, ray, transformMatrix);
	};

	
    glm::vec2 getUVCoordinates(glm::vec3 ray) {
        glm::vec2 uv;
        
        // Check if we're on the base cap (y = -0.5)
        const float EPSILON = 1e-4f;

        if (std::abs(ray.y + 0.5f) < EPSILON) {
            // Planar mapping for base
            uv.x = (-ray.x  / ray.y+ 1.0f) * 0.5f;
            uv.y = (ray.z / ray.y+ 1.0f) * 0.5f;
        } else {
            // Body mapping
            float theta = atan2(-ray.z, ray.x);
            uv.x =  (theta + M_PI) / (2.0f * M_PI);    
            uv.y = (ray.y + 0.5f);
        }
        
        return uv;
    }


    glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 worldSpacePos, glm::mat4 transformMatrix, double t) {
        glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
        // Convert eye point to object space - should use w=1.0 for rays
        glm::vec4 objP = inverseMatrix * glm::vec4(eyePoint, 1.0f);
        // Convert ray to object space - should use w=0.0 for vectors
        glm::vec4 intersectPoint = inverseMatrix * glm::vec4(worldSpacePos, 1.0f);

        // for a cone, we need to check 2 cases: 1) base cap, 2) cone body
        glm::vec3 normalObj;
        if (std::abs(intersectPoint.y + 0.5f) < 0.0001f) { // base cap 
            normalObj = glm::vec3(0, -1, 0);
        }
        else { // cone body
            // equation: n=∇f(x,y,z)=(2x,−2(r/h)^2 * y, 2z)
            float r = 0.5f; // radius for unit cone 
            float h = 1.0f; // height for unit cone 
            float k = (r * r) / (h * h); // (r^2/h^2) for unit cone 
            normalObj = glm::normalize(glm::vec3(
                intersectPoint.x,
                -k * (intersectPoint.y - h / 2.0f), // center at 0 
                intersectPoint.z));
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