#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#define DEFAULT_FOCUS_LENGTH 1.0f
#define NEAR_PLANE 0.01f
#define FAR_PLANE 20.0f
#define VIEW_ANGLE 60.0f


class Camera {
public:
	float viewAngle, filmPlanDepth;
	float nearPlane, farPlane;
	int screenWidth, screenHeight;
	float screenWidthRatio; // m_screenHeightRatio;
	float rotU, rotV, rotW;

	Camera();
	~Camera();
	void reset();
	void orientLookAt(glm::vec3 eyePoint, glm::vec3 focusPoint, glm::vec3 upVec);
	void orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec);
	void setViewAngle (float _viewAngle);
	void setNearPlane (float _nearPlane);
	void setFarPlane (float _farPlane);
	void setScreenSize (int _screenWidth, int _screenHeight);

	glm::mat4 getModelViewMatrix();

	glm::mat4 getScaleMatrix();
	glm::mat4 getInverseScaleMatrix();
	glm::mat4 getUnhingeMatrix();
	glm::mat4 getProjectionMatrix();

	glm::mat4 getInverseModelViewMatrix();

	void setRotUVW(float u, float v, float w);

	void rotateV(float angle);
	void rotateU(float angle);
	void rotateW(float angle);

	void translate(glm::vec3 v);
	void rotate(glm::vec3 point, glm::vec3 axis, float degree);

	glm::vec3 getEyePoint();
	glm::vec3 getLookVector();
	glm::vec3 getUpVector();
	float getViewAngle();
	float getNearPlane();
	float getFarPlane();
	int getScreenWidth();
	int getScreenHeight();

	float getFilmPlanDepth();
	float getScreenWidthRatio();



private:
	bool isNullVector(glm::vec3 v);

	glm::mat4 worldToCameraMat4; //World to camera matrix
	glm::mat4 cameraToWorldMat4; //camera to world matrix
	//Matrix m_projection; //Projection matrix onto film plane.
	glm::vec3 nVec3, uVec3, vVec3;  //n u v of the camera, or u, v, w 

	glm::vec3 eyePoint;
	glm::vec3 lookVector;
	glm::vec3 upVector;
	glm::vec3 u, v, w; // Camera coordinate system
	void updateCameraCoordinates();
};
#endif

