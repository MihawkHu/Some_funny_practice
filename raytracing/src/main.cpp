#include "camera.h"
#include "scene.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <time.h>
#include <glm/glm.hpp>
#include <omp.h>

const std::string currentDateTime();
int savePPM(const char* file_name, const int width, const int height, unsigned char* data);

int main(int argc, char const *argv[])
{
    std::cout << std::endl;
    std::cout << "===== Ray Trace =====" << std::endl;
    std::cout << "Name: Hu Hu" << std::endl;
    std::cout << "Student ID: 5140519019" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << std::endl;
    
    time_t time_start, time_now, rendertime_start;
    time(&time_start);

    static const int WIDTH = 1024 / 1;
    static const int HEIGHT = 768 / 1;
    static const int SUB_SAMPLING_CAUSTICS = 10;
    static const int SUB_SAMPLING_MONTE_CARLO = 500;
    static const int SUB_SAMPLING_DIRECT_SPECULAR = 100;
    static const int NUMBER_OF_PHOTONS_EMISSION = 2000000;

    Camera c(glm::vec3(0, 0, 3.2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), M_PI / 3, WIDTH, HEIGHT); 
    glm::vec3 camera_plane_normal = glm::normalize(c.center - c.eye);
    
    Scene s(argv[1]);
    SpectralDistribution* irradiance_values = new SpectralDistribution[c.WIDTH * c.HEIGHT];
    unsigned char* pixel_values = new unsigned char[c.WIDTH * c.HEIGHT * 3];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-0.5, 0.5);

    std::cout << "Start building." << std::endl;
    s.buildPhotonMap(NUMBER_OF_PHOTONS_EMISSION);

    float rendering_percent_finished = 0;
    std::cout << "Start rendering." << std::endl;
    std::cout << rendering_percent_finished << " \% has finished." << std::endl;

    time(&rendertime_start);
    double prerender_time = difftime(rendertime_start, time_start);

    for (int x = 0; x < c.WIDTH; ++x) {
        // openMP parallellize
        #pragma omp parallel for
        for (int y = 0; y < c.HEIGHT; ++y)
        {
            int index = (x + y * c.WIDTH);
            SpectralDistribution sd;
            if (SUB_SAMPLING_DIRECT_SPECULAR)
            {
                for (int i = 0; i < SUB_SAMPLING_DIRECT_SPECULAR; ++i)
                {
                    Ray r = c.castRay(x, (c.HEIGHT - y - 1), dis(gen), dis(gen));
                    sd += s.traceRay(r, Scene::WHITTED_SPECULAR) * glm::dot(r.direction, camera_plane_normal);
                }
                irradiance_values[index] += sd / SUB_SAMPLING_DIRECT_SPECULAR * (2 * M_PI);                
            }
            sd = SpectralDistribution();
            if (SUB_SAMPLING_CAUSTICS)
                {
                for (int i = 0; i < SUB_SAMPLING_CAUSTICS; ++i)
                {
                    Ray r = c.castRay(x, (c.HEIGHT - y - 1), dis(gen), dis(gen));
                    sd += s.traceRay(r, Scene::CAUSTICS) * glm::dot(r.direction, camera_plane_normal);
                }
                irradiance_values[index] += sd / SUB_SAMPLING_CAUSTICS * (2 * M_PI);
            }
            sd = SpectralDistribution();
            if (SUB_SAMPLING_MONTE_CARLO)
                {
                for (int i = 0; i < SUB_SAMPLING_MONTE_CARLO; ++i)
                {
                    Ray r = c.castRay(x, (c.HEIGHT - y - 1), dis(gen), dis(gen));
                    sd += s.traceRay(r, Scene::MONTE_CARLO) * glm::dot(r.direction, camera_plane_normal);
                }
                irradiance_values[index] += sd / SUB_SAMPLING_MONTE_CARLO * (2 * M_PI);
            }
        }
        rendering_percent_finished = (x+1) * 100 / float(c.WIDTH);
          time(&time_now);
        std::cout << rendering_percent_finished << " \% has finished." << std::endl;
    }

    float gamma = 1 / 2.2;
    for (int x = 0; x < c.WIDTH; ++x)
    {
        for (int y = 0; y < c.HEIGHT; ++y)
        {
            int index = (x + y * c.WIDTH);
            pixel_values[index * 3 + 0] = char(int(glm::clamp(
                glm::pow(irradiance_values[index][0],gamma), 0.0f, 1.0f) * 255));
            pixel_values[index * 3 + 1] = char(int(glm::clamp(
                glm::pow(irradiance_values[index][1],gamma), 0.0f, 1.0f) * 255));
            pixel_values[index * 3 + 2] = char(int(glm::clamp(
                glm::pow(irradiance_values[index][2],gamma), 0.0f, 1.0f) * 255));
        }
    }

    std::string date_time = currentDateTime();
    std::string file_name = std::string(argv[1]) + std::string(".ppm");
    savePPM(file_name.c_str(), WIDTH, HEIGHT, pixel_values);
    std::cout << "Saving result file " << file_name << std::endl;
    std::cout << "===== Mission Done =====" << std::endl;
    
    delete [] irradiance_values;
    delete [] pixel_values;
  
    return 0;
}

int savePPM(const char* file_name, const int width, const int height, unsigned char* data) {
    FILE *fp = fopen(file_name, "wb");
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    fwrite(data, 1, width * height * 3, fp);
    fclose(fp);
    return 0;
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d-%X", &tstruct);
    return buf;
}