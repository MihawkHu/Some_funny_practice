#ifndef _CAMERA_H
#define _CAMERA_H
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "tool.h"

class Camera {
public:
    const int HEIGHT;
    const int WIDTH;
    
    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;
    float fov;
    glm::mat4 VP_inv;


    Camera(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up, const float fov, const int width, const int height);
    ~Camera(){};

    Ray castRay(const int pixel_x, const int pixel_y, const float parameter_x, const float parameter_y);

    int getWidth();
    int getHeight();
};

#endif