#include "Camera.h"
#include <OpenGL/OpenGL.h>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera() { reset(); }

Camera::~Camera() {}

void Camera::reset() {
  orientLookAt(glm::vec3(0.0f, 0.0f, DEFAULT_FOCUS_LENGTH),
               glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  setViewAngle(VIEW_ANGLE);
  setNearPlane(NEAR_PLANE);
  setFarPlane(FAR_PLANE);
  screenWidth = screenHeight = 200;
  screenWidthRatio = 1.0f;
  rotU = rotV = rotW = 0;
}

// called by main.cpp as a part of the slider callback for controlling rotation
//  the reason for computing the diff is to make sure that we are only
//  incrementally rotating the camera
void Camera::setRotUVW(float u, float v, float w) {
  float diffU = u - rotU;
  float diffV = v - rotV;
  float diffW = w - rotW;
  rotateU(diffU);
  rotateV(diffV);
  rotateW(diffW);
  rotU = u;
  rotV = v;
  rotW = w;
}

void Camera::orientLookAt(glm::vec3 eyePoint, glm::vec3 lookatPoint,
                          glm::vec3 upVec) {
  orientLookVec(eyePoint, lookatPoint - eyePoint, upVec);
}

void Camera::orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec,
                           glm::vec3 upVec) {
  position = eyePoint;
  lookVector = glm::normalize(lookVec);
  glm::vec3 right = glm::normalize(glm::cross(lookVector, upVec));
  upVector = glm::normalize(glm::cross(right, lookVector));
}

glm::mat4 Camera::getScaleMatrix() {
  glm::mat4 scaleMat4(1.0);

  float viewAngle = glm::radians(getViewAngle()); 
  float nearPlane = getNearPlane();              
  float farPlane = getFarPlane();               
  int screenWidth = getScreenWidth();          
  int screenHeight = getScreenHeight();       

  if (screenWidth <= 0 || screenHeight <= 0) {
    std::cerr << "Error: Screen dimensions must be positive." << std::endl;
    screenWidth = (screenWidth <= 0) ? 1 : screenWidth;
    screenHeight = (screenHeight <= 0) ? 1 : screenHeight;
  }

  float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

  if (viewAngle <= 0.0f || viewAngle >= glm::radians(180.0f)) {
    std::cerr << "Error: Invalid view angle." << std::endl;
    viewAngle = glm::radians(45.0f);
  }

  if (nearPlane <= 0.0f || nearPlane >= farPlane) {
    std::cerr << "Error: Invalid near or far plane values." << std::endl;
    nearPlane = 0.1f;
    farPlane = 100.0f;
  }

  float h_half = glm::tan(viewAngle / 2) * farPlane;
  float w_half = h_half * aspectRatio;
  scaleMat4[0][0] = 1 / w_half;
  scaleMat4[1][1] = 1 / h_half;
  scaleMat4[2][2] = 1 / farPlane;
  return scaleMat4;
}

glm::mat4 Camera::getInverseScaleMatrix() {
  glm::mat4 invScaleMat4(1.0);
  float viewAngle = getViewAngle();
  float nearPlane = getNearPlane();
  float farPlane = getFarPlane();
  int screenWidth = getScreenWidth();
  int screenHeight = getScreenHeight();
  float aspectRatio =
      static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

  float h_half = glm::tan(viewAngle / 2) * farPlane;
  float w_half = h_half * aspectRatio;
  invScaleMat4[0][0] = w_half;
  invScaleMat4[1][1] = h_half;
  invScaleMat4[2][2] = farPlane;
  return invScaleMat4;
}

glm::mat4 Camera::getUnhingeMatrix() {
  glm::mat4 unhingeMat4(1.0);
  float nearPlane = getNearPlane(); 
  float farPlane = getFarPlane();  
  float c = -(nearPlane / farPlane);

  if (nearPlane <= 0.0f || nearPlane >= farPlane) {
    std::cerr << "Error: Invalid near or far plane values." << std::endl;
    nearPlane = 0.1f;
    farPlane = 100.0f;
  }
  unhingeMat4[2][2] = -(1 / (c + 1));
  unhingeMat4[3][3] = 0;
  unhingeMat4[2][3] = -1;
  unhingeMat4[3][2] = c / (c + 1);
  return unhingeMat4;
}

glm::mat4 Camera::getProjectionMatrix() {
  glm::mat4 projectionMatrix = getUnhingeMatrix() * getScaleMatrix();
  return projectionMatrix;
}

glm::mat4 Camera::getInverseModelViewMatrix() {
  glm::vec3 eyePoint = getEyePoint();
  glm::vec3 lookVector = getLookVector();
  glm::vec3 upVector = getUpVector();

  glm::vec3 target = eyePoint + lookVector;

  glm::mat4 viewMatrix = glm::lookAt(eyePoint, target, upVector);
  glm::mat4 modelViewMat4 = viewMatrix;
  
  return glm::inverse(modelViewMat4);
}

void Camera::setViewAngle(float _viewAngle) {
  if (_viewAngle <= 0.0f || _viewAngle >= 180.0f) {
    std::cerr << "Error: View angle must be between 0 and 180 degrees. "
              << "Setting to default 60 degrees." << std::endl;
    viewAngle = glm::radians(60.0f);
  } else {
    viewAngle = glm::radians(_viewAngle);
  }
}

void Camera::setNearPlane(float _nearPlane) {
  if (_nearPlane <= 0.0f) {
    std::cerr << "Error: Near plane must be greater than 0. "
              << "Setting to default 0.01f." << std::endl;
    nearPlane = 0.01f;
  } else {
    nearPlane = _nearPlane;
  }
}

void Camera::setFarPlane(float _farPlane) {
  if (_farPlane <= nearPlane) {
    std::cerr << "Error: Far plane must be greater than near plane. "
              << "Setting to default 20.0f." << std::endl;
    farPlane = 20.0f;
  } else {
    farPlane = _farPlane;
  }
}

void Camera::setScreenSize(int _screenWidth, int _screenHeight) {
  if (_screenWidth <= 0 || _screenHeight <= 0) {
    std::cerr << "Error: Screen dimensions must be positive. "
              << "Setting to default 800x600." << std::endl;
    screenWidth = 800;
    screenHeight = 600;
  } else {
    screenWidth = _screenWidth;
    screenHeight = _screenHeight;
  }
}

glm::mat4 Camera::getModelViewMatrix() {
  glm::vec3 eyePoint = getEyePoint();
  glm::vec3 lookVector = getLookVector();
  glm::vec3 upVector = getUpVector();

  glm::vec3 target = eyePoint + lookVector;

  glm::mat4 viewMatrix = glm::lookAt(eyePoint, target, upVector);
  glm::mat4 modelViewMat4 = viewMatrix;

  return modelViewMat4;
}

void Camera::rotateV(float degrees) {
  float radians = glm::radians(degrees);

  if (glm::length(lookVector) == 0.0f || glm::length(upVector) == 0.0f) {
    std::cerr << "Error: lookVector or upVector is a zero vector!" << std::endl;
    return;
  }

  glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, upVector);

  glm::vec3 newLook = glm::vec3(rotationMatrix * glm::vec4(lookVector, 0.0f));
  glm::vec3 newRight = glm::normalize(glm::cross(newLook, upVector));

  lookVector = glm::normalize(newLook);
}

void Camera::rotateU(float degrees) {
  float radians = glm::radians(degrees);

  if (glm::length(lookVector) == 0.0f || glm::length(upVector) == 0.0f) {
    std::cerr << "Error: lookVector or upVector is a zero vector!" << std::endl;
    return;
  }

  glm::vec3 right = glm::cross(lookVector, upVector);
  if (glm::length(right) == 0.0f) {
    std::cerr << "Error: lookVector and upVector are parallel!" << std::endl;
    return;
  }
  right = glm::normalize(right);

  glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, right);

  glm::vec3 newLook = glm::vec3(rotationMatrix * glm::vec4(lookVector, 0.0f));
  glm::vec3 newUp = glm::vec3(rotationMatrix * glm::vec4(upVector, 0.0f));

  lookVector = glm::normalize(newLook);
  upVector = glm::normalize(newUp);
}

void Camera::rotateW(float degrees) {

  float radians = glm::radians(degrees);

  if (glm::length(lookVector) == 0.0f || glm::length(upVector) == 0.0f) {
    std::cerr << "Error: lookVector or upVector is a zero vector!" << std::endl;
    return;
  }

  glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, lookVector);
  glm::vec3 newUp = glm::vec3(rotationMatrix * glm::vec4(upVector, 0.0f));
  glm::vec3 newRight = glm::normalize(glm::cross(lookVector, newUp));

  upVector = glm::normalize(newUp);
}

void Camera::translate(glm::vec3 v) {
    glm::vec3 globalTransMat = getInverseModelViewMatrix() * glm::vec4(v, 0.0f);
    lookVector = lookVector + globalTransMat ;
}

void Camera::rotate(glm::vec3 point, glm::vec3 axis, float degrees) {
  float radians = glm::radians(degrees);
  glm::mat4 rotationMatrix = glm::translate(glm::mat4(1.0f), point) *
                             glm::rotate(glm::mat4(1.0f), radians, axis) *
                             glm::translate(glm::mat4(1.0f), -point);

  lookVector =
      glm::normalize(glm::vec3(rotationMatrix * glm::vec4(lookVector, 0.0f)));
  upVector =
      glm::normalize(glm::vec3(rotationMatrix * glm::vec4(upVector, 0.0f)));
}

glm::vec3 Camera::getEyePoint() { return position; }

glm::vec3 Camera::getLookVector() { return lookVector; }

glm::vec3 Camera::getUpVector() { return upVector; }

float Camera::getViewAngle() { return glm::degrees(viewAngle); }

float Camera::getNearPlane() { return nearPlane; }

float Camera::getFarPlane() { return farPlane; }

int Camera::getScreenWidth() { return screenWidth; }

int Camera::getScreenHeight() { return screenHeight; }

float Camera::getScreenWidthRatio() { return screenWidthRatio; }

// Logs
bool isZeroVector(const glm::vec3 &vec) {
  return vec.x == 0.0f && vec.y == 0.0f && vec.z == 0.0f;
}

void Camera::printCamVec(const std::string &info) {
  std::printf("%s\n", info.c_str());
  if (isZeroVector(position)) {
    std::printf("Warning: position is uninitialized or zero vector!\n");
  } else {
    std::printf("position: (%f, %f, %f)\n", position.x, position.y, position.z);
  }

  // print lookVector
  if (isZeroVector(lookVector)) {
    std::printf("Warning: lookVector is uninitialized or zero vector!\n");
  } else {
    std::printf("lookVector: (%f, %f, %f)\n", lookVector.x, lookVector.y,
                lookVector.z);
  }

  // print upVector
  if (isZeroVector(upVector)) {
    std::printf("Warning: upVector is uninitialized or zero vector!\n");
  } else {
    std::printf("upVector: (%f, %f, %f)\n", upVector.x, upVector.y, upVector.z);
  }
}

void Camera::printMat4(const glm::mat4 &mat) {
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      std::cout << mat[row][col] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
