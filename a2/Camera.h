#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

const float EPSILON = 1e-20;
#define IN_RANGE(a, b) (((a > (b - EPSILON)) && (a < (b + EPSILON))) ? 1 : 0)

#define DEFAULT_FOCUS_LENGTH 1.0f
#define NEAR_PLANE 0.01f
#define FAR_PLANE 20.0f
#define VIEW_ANGLE 60.0f
#define PI glm::pi<float>()

class Camera {
public:
  float rotU, rotV, rotW; // values used by the callback in main.cpp

  Camera();
  ~Camera();

  void reset();
  void orientLookAt(glm::vec3 eyePoint, glm::vec3 focusPoint, glm::vec3 upVec);
  void orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec);
  void setViewAngle(float _viewAngle);
  void setNearPlane(float _nearPlane);
  void setFarPlane(float _farPlane);
  void setScreenSize(int _screenWidth, int _screenHeight);
  void
  setRotUVW(float u, float v,
            float w); // called by main.cpp as a part of the slider callback

  glm::mat4 getUnhingeMatrix();
  glm::mat4 getProjectionMatrix();
  glm::mat4 getScaleMatrix();
  glm::mat4 getInverseScaleMatrix();
  glm::mat4 getModelViewMatrix();
  glm::mat4 getInverseModelViewMatrix();

  void rotateV(float degree);
  void rotateU(float degree);
  void rotateW(float degree);
  void rotate(glm::vec3 point, glm::vec3 axis, float degree);

  void translate(glm::vec3 v);

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
  glm::vec3 position;
  glm::vec3 lookVector;
  glm::vec3 upVector;

  float viewAngle, filmPlanDepth;
  float nearPlane, farPlane;
  int screenWidth, screenHeight;
  float screenWidthRatio;

  void printCamVec(const std::string &info);
  void printMat4(const glm::mat4 &mat);
};
#endif
