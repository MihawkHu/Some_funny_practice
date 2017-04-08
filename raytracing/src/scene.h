#ifndef SCENE_H
#define SCENE_H
#include "object.h"
#include "tool.h"
#include <glm/glm.hpp>
#include "lib/include/kdtree++/kdtree.hpp"
#include <map>
#include <vector>
#include <random>

class Scene {
private:
    std::vector<Object3D*> objects_;
    std::vector<LightSource*> lamps_;
    std::map<std::string, Material*> materials_;
    std::random_device rd_;
    std::mt19937* gen_;
    std::uniform_real_distribution<float>* dis_;
    KDTree::KDTree<3, KDTreeNode> photon_map_;

    friend struct scene_traverser;
    
    SpectralDistribution traceDiffuseRay(Ray r, int render_mode, IntersectionData id, int iteration);
    SpectralDistribution traceLocalDiffuseRay(Ray r, int render_mode, IntersectionData id);
    SpectralDistribution traceIndirectDiffuseRay(Ray r, int render_mode, IntersectionData id, int iteration);
    
    SpectralDistribution traceSpecularRay(Ray r, int render_mode, IntersectionData id, int iteration);
    SpectralDistribution traceRefractedRay(Ray r, int render_mode, IntersectionData id, int iteration, glm::vec3 offset, bool inside);

    bool intersect(IntersectionData* id, Ray r);
    bool intersectLamp(LightSourceIntersectionData* light_id, Ray r);
    glm::vec3 shake(glm::vec3 r, float power);
public:
    Scene(const char* file_path);
    ~Scene();
    
    enum RenderMode{
      PHOTON_MAPPING, CAUSTICS, WHITTED_SPECULAR, MONTE_CARLO,
    };
    
    SpectralDistribution traceRay(Ray r, int render_mode, int iteration = 0);
    void buildPhotonMap(const int n_photons);

    int getNumberOfTriangles();
    int getNumberOfObjects();
    int getNumberOfSpheres();
    int getNumberOfPhotons();
};

#endif