#include "camera.h"
#include <iostream>

Camera::Camera (glm::vec3 eye, glm::vec3 center, glm::vec3 up, float fov, int width, int height) :
    eye(eye), center(center), up(up), fov(fov), WIDTH(width), HEIGHT(height)
{
    glm::mat4 V = glm::lookAt(eye, center, up);
    float aspect = float(WIDTH) / HEIGHT;
    glm::mat4 P = glm::perspective(fov, aspect, 0.1f, 100.0f);
    VP_inv = glm::inverse(V * P);
}

Ray Camera::castRay(int pixel_x, int pixel_y, float parameter_x, float parameter_y)
{
    Ray r;
    if (pixel_x < 0 || pixel_x > WIDTH - 1 ||
        pixel_y < 0 || pixel_y > HEIGHT - 1 ||
        parameter_x < -0.5 || parameter_x > 0.5 ||
        parameter_y < -0.5 || parameter_y > 0.5
        )
    {
        std::cout << "ERROR : Invalid arguments in castRay()" << std::endl;
        r.origin = glm::vec3(0);
        r.direction = glm::vec3(0);
    }
    else
    {
        glm::vec4 from4 = VP_inv * glm::vec4(((pixel_x + parameter_x) / WIDTH - 0.5) * 2, ((pixel_y + parameter_y) / HEIGHT - 0.5) * 2, 1, 1 );
        glm::vec4 to4 = VP_inv * glm::vec4(((pixel_x + parameter_x) / WIDTH - 0.5) * 2, ((pixel_y + parameter_y) / HEIGHT - 0.5) * 2, -1, 1 );
        glm::vec3 from = glm::vec3(from4) * from4.w;
        glm::vec3 to = glm::vec3(to4) * to4.w;
        glm::vec3 direction = glm::normalize(to - from);

        r.origin = eye;
        r.direction = direction;
        r.material = Material::air();
        r.radiance = SpectralDistribution();
        r.radiance[0] = 1;
        r.radiance[1] = 1;
        r.radiance[2] = 1;
    }
    return r;
}