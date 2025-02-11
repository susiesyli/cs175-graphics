#include "Camera.h"

Camera::Camera()
{
	reset();
}

Camera::~Camera()
{
}

void Camera::reset()
{
	orientLookAt(glm::vec3(0.0f, 0.0f, DEFAULT_FOCUS_LENGTH), // eyePoint
		glm::vec3(0.0f, 0.0f, 0.0f), // focusPoint
		glm::vec3(0.0f, 1.0f, 0.0f)); // upVector
	setViewAngle(VIEW_ANGLE);
	setNearPlane(NEAR_PLANE);
	setFarPlane(FAR_PLANE);
	screenWidth = screenHeight = 200;
	screenWidthRatio = 1.0f;
	rotU = rotV = rotW = 0;
}

// called by main.cpp as a part of the slider callback for controlling rotation
// the reason for computing the diff is to make sure that we are only incrementally rotating the camera
void Camera::setRotUVW(float u, float v, float w)
{
	rotateU(u - rotU);
	rotateV(v - rotV);
	rotateW(w - rotW);
	rotU = u;
	rotV = v;
	rotW = w;
}

void Camera::orientLookAt(glm::vec3 eyePoint, glm::vec3 focusPoint, glm::vec3 upVec)
{
	this->eyePoint = eyePoint;
	this->lookVector = glm::normalize(focusPoint - eyePoint);
	this->upVector = glm::normalize(upVec);
	updateCameraCoordinates();
}

void Camera::orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec)
{
	this->eyePoint = eyePoint;
	this->lookVector = glm::normalize(lookVec);
	this->upVector = glm::normalize(upVec);
	updateCameraCoordinates();
}

// Derived from 07-camera-handout.pptx page 32-35/120
void Camera::updateCameraCoordinates()
{
	w = -lookVector / glm::length(lookVector);
	u = glm::normalize(glm::cross(upVector, w));
	v = glm::cross(w, u);
}

void Camera::setViewAngle(float _viewAngle)
{
	viewAngle = _viewAngle;
	filmPlanDepth = 1.0f / tan(glm::radians(viewAngle) / 2.0f);
}

void Camera::setNearPlane(float _nearPlane)
{
	nearPlane = _nearPlane;
}

void Camera::setFarPlane(float _farPlane)
{
	farPlane = _farPlane;
}

void Camera::setScreenSize(int _screenWidth, int _screenHeight)
{
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;
	screenWidthRatio = (float)screenWidth / (float)screenHeight;
}

// Derived from 07-camera-handout.pptx page 95/120
glm::mat4 Camera::getUnhingeMatrix()
{
	float c = -(nearPlane / farPlane);
	return glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f / (c + 1), -1,
		0.0f, 0.0f, c / (c + 1), 0.0f
	);
}

// Derived from slide 106
glm::mat4 Camera::getProjectionMatrix()
{
	return getUnhingeMatrix() * getScaleMatrix();
}

// Derived from slide 106
glm::mat4 Camera::getModelViewMatrix()
{
	return (glm::transpose(glm::mat4(1.0f) * glm::mat4(glm::mat3(u, v, w))) * glm::translate(glm::mat4(1.0f), -eyePoint));
}

// Derived from slide 106
glm::mat4 Camera::getScaleMatrix()
{
	// float scalex = 1 / (tan(glm::radians((viewAngle / screenWidthRatio) / 2.0f)) * farPlane);
	// float scaley = 1 / (tan(glm::radians(viewAngle / 2.0f)) * farPlane);
    float scaley = 1 / (tan(glm::radians(viewAngle / 2.0f)) * farPlane);
    float scalex = scaley / screenWidthRatio;
    float scalez = 1 / farPlane;
    
	return glm::scale(glm::mat4(1.0f), glm::vec3(scalex, scaley, scalez));
}

glm::mat4 Camera::getInverseScaleMatrix()
{
	return glm::inverse(getScaleMatrix());
}

glm::mat4 Camera::getInverseModelViewMatrix()
{
	return glm::inverse(getModelViewMatrix());
}

void Camera::rotateV(float degrees)
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), v);
	lookVector = glm::vec3(rotation * glm::vec4(lookVector, 0.0f));
	upVector = glm::vec3(rotation * glm::vec4(upVector, 0.0f));
	updateCameraCoordinates();
}

void Camera::rotateU(float degrees)
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), u);
	lookVector = glm::vec3(rotation * glm::vec4(lookVector, 0.0f));
	upVector = glm::vec3(rotation * glm::vec4(upVector, 0.0f));
	updateCameraCoordinates();
}

void Camera::rotateW(float degrees)
{
	degrees = -degrees;
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), w);
	lookVector = glm::vec3(rotation * glm::vec4(lookVector, 0.0f));
	upVector = glm::vec3(rotation * glm::vec4(upVector, 0.0f));
	updateCameraCoordinates();
}

void Camera::rotate(glm::vec3 point, glm::vec3 axis, float degrees)
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis);
	glm::vec3 translatedEye = eyePoint - point;
	translatedEye = glm::vec3(rotation * glm::vec4(translatedEye, 0.0f));
	eyePoint = translatedEye + point;
	lookVector = glm::vec3(rotation * glm::vec4(lookVector, 0.0f));
	upVector = glm::vec3(rotation * glm::vec4(upVector, 0.0f));
	updateCameraCoordinates();
}

void Camera::translate(glm::vec3 v)
{
	eyePoint += v;
}

glm::vec3 Camera::getEyePoint()
{
	return eyePoint;
}

glm::vec3 Camera::getLookVector()
{
	return lookVector;
}

glm::vec3 Camera::getUpVector()
{
	return upVector;
}

float Camera::getViewAngle()
{
	return viewAngle;
}

float Camera::getNearPlane()
{
	return nearPlane;
}

float Camera::getFarPlane()
{
	return farPlane;
}

int Camera::getScreenWidth()
{
	return screenWidth;
}

int Camera::getScreenHeight()
{
	return screenHeight;
}

float Camera::getFilmPlanDepth()
{
	return filmPlanDepth;
}

float Camera::getScreenWidthRatio()
{
	return screenWidthRatio;
}