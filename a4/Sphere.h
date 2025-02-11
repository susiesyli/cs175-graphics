#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere : public Shape {
public:
	Sphere() {};
	~Sphere() {};

	OBJ_TYPE getType() {
		return SHAPE_SPHERE;
	}

    glm::vec2 getUVCoordinates(glm::vec3 point) {
        glm::vec2 uv;
        
        // u = θ/2π (longitude), where θ = atan2(z, x)
        float theta = atan2(-point.z, point.x);
        uv.x = (theta + M_PI) / (2.0f * M_PI);
        float phi = asin(point.y / 0.5f);
        uv.y = (phi / M_PI) + 0.5f;
        
        // Handle singularity at poles (v = 0 or v = 1)
        if (std::abs(point.y) > 0.499f) {
            uv.x = 0.5f;  // Set a default u value at poles
        }        
        return uv;
    }

    // glm::vec2 getUVCoordinates(glm::vec3 point) {
    //     // For a sphere, we use spherical coordinates:
    //     // u = θ/2π + 0.5   (longitude angle mapping)
    //     // v = φ/π + 0.5    (latitude angle mapping)
        
    //     // Calculate θ (longitude angle in [-π, π])
    //     float theta = atan2(point.z, point.x);
        
    //     // Calculate φ (latitude angle in [-π/2, π/2])
    //     float phi = asin(point.y);  // point should be normalized
        
    //     // Map to UV coordinates
    //     float u = (theta + M_PI) / (2.0f * M_PI);  // map [-π,π] to [0,1]
    //     float v = (phi + M_PI/2.0f) / M_PI;        // map [-π/2,π/2] to [0,1]
        
    //     return glm::vec2(u, v);
    // }

	float draw(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix) {
	/*	std::cout << "IN SPHERE DRAW" << "\n";
		std::cout << ray.x << "\n";*/
		float t = intersect(eyePoint, ray, transformMatrix);
		return t;
	};

	//// only called for the smallest non-negative t value
 //   glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix, double t) {
 //       glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
 //       // Convert eye point to object space - should use w=1.0 for points
 //       glm::vec4 objP = inverseMatrix * glm::vec4(eyePoint, 1.0f);
 //       // Convert ray to object space - should use w=0.0 for vectors
 //       glm::vec4 objRay = inverseMatrix * glm::vec4(ray, 0.0f);
 //       // Calculate intersection point in object coordinates
 //       glm::vec3 objRayNorm = glm::normalize(glm::vec3(objRay));
 //       glm::vec3 intersectPoint = glm::vec3(objP) + float(t) * objRayNorm;

 //       // For a sphere, the normal is just the normalized point (since it's centered at origin)
 //       glm::vec3 normalObj = glm::normalize(intersectPoint);

 //       // Transform normal to world coordinates using the normal transform matrix
 //       // For normals, we need to use the transpose of the inverse of the 3x3 part
 //       glm::mat3 normalMatrix = glm::transpose(glm::mat3(inverseMatrix));
 //       glm::vec3 worldNormal = glm::normalize(normalMatrix * normalObj);

 //       return worldNormal;
 //   }

	glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 worldSpacePos, glm::mat4 transformMatrix, double t) {
		glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
		// Convert eye point to object space - should use w=1.0 for points
		glm::vec4 objP = inverseMatrix * glm::vec4(eyePoint, 1.0f);
		// Convert ray to object space - should use w=0.0 for vectors
		glm::vec4 intersectPoint = inverseMatrix * glm::vec4(worldSpacePos, 1.0f);

		// For a sphere, the normal is just the normalized point (since it's centered at origin)
		glm::vec3 normalObj = glm::normalize(intersectPoint);

		// Transform normal to world coordinates using the normal transform matrix
		// For normals, we need to use the transpose of the inverse of the 3x3 part
		glm::mat3 normalMatrix = glm::transpose(glm::mat3(inverseMatrix));
		glm::vec3 worldNormal = glm::normalize(normalMatrix * normalObj);

		return worldNormal;
	}


	/* The intersect function accepts three input parameters:
	(1) the eye point (in world coordinate)
	(2) the ray vector (in world coordinate)
	(3) the transform matrix that would be applied to there sphere to transform it from object coordinate to world coordinate

	The function should return:
	(1) a -1 if no intersection is found
	(2) OR, the "t" value which is the distance from the origin of the ray to the (nearest) intersection point on the sphere
*/
	float intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix) {
		glm::mat4 inverseMatrix = glm::inverse(transformMatrix);

		// Convert to object space
		glm::vec4 objP = inverseMatrix * glm::vec4(eyePointP, 1.0f);
		glm::vec4 objRay = inverseMatrix * glm::vec4(rayV, 0.0f);

		glm::vec3 P = glm::vec3(objP);
		glm::vec3 d = glm::vec3(objRay);

		//// Convert eye point to object space - should use w=1.0 for points
		//glm::vec4 objP = inverseMatrix * glm::vec4(eyePointP, 1.0f);

		//// Convert ray to object space - should use w=0.0 for vectors
		//glm::vec4 objRay = inverseMatrix * glm::vec4(glm::normalize(rayV), 0.0f);

		//// Normalize the ray direction after transformation
		//glm::vec3 d = glm::vec3(objRay);

		float A = glm::dot(d, d);
		float B = 2.0f * glm::dot(P, d);
		float C = glm::dot(P, P) - 0.25f;

		float discriminant = (B * B) - (4.0f * A * C);

		if (discriminant < 0) {
			return -1;
		}

		float t1 = (-B + sqrt(discriminant)) / (2.0f * A);
		float t2 = (-B - sqrt(discriminant)) / (2.0f * A);

		if (t1 < 0 && t2 < 0)
			return -1;
		else if (t1 < 0)
			return t2;
		else if (t2 < 0)
			return t1;

		return std::min(t1, t2);
	}

private:
};

#endif