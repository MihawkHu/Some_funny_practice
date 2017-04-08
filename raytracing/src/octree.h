#ifndef _OCTTREE_H
#define _OCTTREE_H
#include <vector>
#include <glm/glm.hpp>
#include "tool.h"

class Mesh;
struct STREE
{
    bool intersect(Ray r) const;
    bool intersectTriangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) const;

    glm::vec3 min_;
    glm::vec3 max_;
};

class OctNodeGG
{
public:
    OctNodeGG(
        OctNodeGG* parent,
        int depth,
        Mesh* mesh,
        glm::vec3 gg_min,
        glm::vec3 gg_max);
    ~OctNodeGG();

    bool intersect(IntersectionData* id, Ray r) const;

protected:
    Mesh* mesh_;
    STREE gg_;
    std::vector<unsigned int> triangle_indices_;

    OctNodeGG* children_[8];
};

class OctTreeGG : public OctNodeGG
{
public:
    OctTreeGG(Mesh* mesh);
    ~OctTreeGG();
private:
};

#endif