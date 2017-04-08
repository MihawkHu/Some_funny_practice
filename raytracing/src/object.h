#ifndef _OBJECT_H
#define _OBJECT_H
#include "octree.h"
#include <vector>
#include <glm/glm.hpp>
#include "tool.h"

class Object3D {
private:
    const Material* material_;
public:
    Object3D(Material* material);
    virtual ~Object3D(){};

    virtual bool intersect(IntersectionData* id, Ray r) const = 0;
    Material material() const;
};

class Mesh : public Object3D
{
private:
    glm::mat4 transform_;
    OctTreeGG* ot_gg_;
    
    std::vector<glm::vec3> positions_;
    std::vector<glm::vec2> uvs_;
    std::vector<glm::vec3> normals_;
    std::vector<unsigned int> indices_;


    friend class OctNodeGG;
public:
    Mesh(glm::mat4 transform, const char* file_path, Material * material);
    ~Mesh(){ delete ot_gg_; };

    virtual bool intersect(IntersectionData* id, Ray r) const;
    glm::vec3 getMinPosition() const;
    glm::vec3 getMaxPosition() const;
    glm::mat4 getTransform() const;
    int getNumberOfTriangles() const;
};

class Sphere : public Object3D
{
private:
    const glm::vec3 POSITION_;
    const float RADIUS_;
public:
    Sphere(glm::vec3 position, float radius, Material* material);
    ~Sphere(){};

    bool intersect(IntersectionData* id, Ray r) const;
    glm::vec3 getPointOnSurface(float u, float v) const;
};

class Plane : public Object3D
{
private:
    const glm::vec3 P0_, P1_, P2_, V1_, V2_, NORMAL_;
    const float AREA_;
public:
    Plane(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Material* material);
    ~Plane(){};

    bool intersect(IntersectionData* id, Ray r) const;
    glm::vec3 getPointOnSurface(float u, float v) const;
    float getArea() const;
    glm::vec3 getNormal() const;
    glm::vec3 getFirstTangent() const;
};

class LightSource
{
private:
    const Plane emitter_;
public:
    LightSource(
        glm::vec3 p0,
        glm::vec3 p1,
        glm::vec3 p2,
        float flux,
        SpectralDistribution color);
    ~LightSource(){};
    
    bool intersect(LightSourceIntersectionData* light_id, Ray r);
    glm::vec3 getPointOnSurface(float u, float v);
    float getArea() const;
    glm::vec3 getNormal() const;

    Ray shootLightRay();

    const SpectralDistribution radiosity;
};

#endif