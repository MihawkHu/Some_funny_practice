#include "octree.h"
#include "object.h"

#include "lib/include/boxOverlap.h"

#include <iostream>

bool STREE::intersectTriangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) const
{
    glm::vec3 center_point = (min_ + max_) / 2.0f;
    glm::vec3 scale = (max_ - center_point) / 1.0f;

    float boxcenter[3] = {center_point[0], center_point[1], center_point[2]};
    float boxhalfsize[3] = {scale[0], scale[1], scale[2]};
    float triverts[3][3] = {{p0.x, p0.y, p0.z}, {p1.x, p1.y, p1.z}, {p2.x, p2.y, p2.z}};

    return triBoxOverlap( boxcenter, boxhalfsize, triverts) == 1;
}

bool STREE::intersect(Ray r) const
{
    glm::vec3 origin = r.origin;
    glm::vec3 direction = r.direction;

    glm::vec3 dirfrac(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);
    float t1 = (min_.x - origin.x)*dirfrac.x;
    float t2 = (max_.x - origin.x)*dirfrac.x;
    float t3 = (min_.y - origin.y)*dirfrac.y;
    float t4 = (max_.y - origin.y)*dirfrac.y;
    float t5 = (min_.z - origin.z)*dirfrac.z;
    float t6 = (max_.z - origin.z)*dirfrac.z;

    float tmin = glm::max(
        glm::max(glm::min(t1, t2), glm::min(t3, t4)),
        glm::min(t5, t6));
    float tmax = glm::min(
        glm::min(glm::max(t1, t2), glm::max(t3, t4)),
        glm::max(t5, t6));

    if (tmax < 0)
    {
        return false;
    }

    if (tmin > tmax)
    {
        return false;
    }

    return true;
    return false;
}

OctTreeGG::OctTreeGG(Mesh* mesh) : 
    OctNodeGG( NULL, 8, mesh, mesh->getMinPosition(), mesh->getMaxPosition()) {}

OctTreeGG::~OctTreeGG() {}

OctNodeGG::OctNodeGG(
    OctNodeGG* parent,
    int depth,
    Mesh* mesh,
    glm::vec3 gg_min,
    glm::vec3 gg_max)
{
    gg_.min_ = gg_min;
    gg_.max_ = gg_max;
    mesh_ = mesh;
    std::vector<unsigned int>& index_list =
        !parent ?
        mesh->indices_ :
        parent->triangle_indices_;
    
    for (int i = 0; i < index_list.size(); i=i+3)
    {
        if (gg_.intersectTriangle(
            mesh->positions_[index_list[i + 0]],
            mesh->positions_[index_list[i + 1]],
            mesh->positions_[index_list[i + 2]]))
        { // Insert the triangle in this node
            triangle_indices_.push_back(index_list[i + 0]);
            triangle_indices_.push_back(index_list[i + 1]);
            triangle_indices_.push_back(index_list[i + 2]);
        }
    }
    
    if (depth == 0 || triangle_indices_.size() <= 3 * 16)
    { // Base case
        for (int i=0; i<8; i++)
            children_[i] = NULL;
    }
    else
    { // Continue recursion, create more children
        glm::vec3 child_gg_min;
        glm::vec3 child_gg_max;
        for (int i = 0; i < 8; ++i)
        {
            child_gg_min = glm::vec3(
                i%2     == 0 ? gg_min.x : (gg_min.x + gg_max.x) / 2,
                (i/2)%2 == 0 ? gg_min.y : (gg_min.y + gg_max.y) / 2,
                (i/4)%2 == 0 ? gg_min.z : (gg_min.z + gg_max.z) / 2);
            child_gg_max = glm::vec3(
                i%2     == 0 ? (gg_min.x + gg_max.x) / 2 : gg_max.x,
                (i/2)%2 == 0 ? (gg_min.y + gg_max.y) / 2 : gg_max.y,
                (i/4)%2 == 0 ? (gg_min.z + gg_max.z) / 2 : gg_max.z);
            children_[i] = new OctNodeGG(
                this,
                depth - 1,
                mesh,
                child_gg_min,
                child_gg_max);
        }
    }
}

OctNodeGG::~OctNodeGG()
{
    for (int i = 0; i < 8; ++i)
    {
        if (children_[i])
            delete children_[i];
    }
}

bool OctNodeGG::intersect(IntersectionData* id, Ray r) const
{
    if (triangle_indices_.size() == 0)
        return false;
    else if (children_[0] == NULL)
    { // Reached a leaf node
        float t_smallest = 10000000;
        bool intersect = false;

        glm::vec3 p0;
        glm::vec3 p1;
        glm::vec3 p2;

        glm::vec3 e1, e2;  //Edge1, Edge2
        glm::vec3 P, Q, T;
        float det, inv_det, u, v;
        float t;

        for (int i = 0; i < triangle_indices_.size(); i=i+3)
        {
            p0 = mesh_->positions_[triangle_indices_[i + 0]];
            p1 = mesh_->positions_[triangle_indices_[i + 1]];
            p2 = mesh_->positions_[triangle_indices_[i + 2]];

            e1 = p1 - p0;
            e2 = p2 - p0;
            P = glm::cross(r.direction, e2);
            det = glm::dot(e1, P);
            if(det > -0.00001 && det < 0.00001) {
                continue;
            }
            
            inv_det = 1.f / det;
            T = r.origin - p0;
            u = glm::dot(T, P) * inv_det;
            if(u < 0.f || u > 1.f) {
                continue;
            }

            Q = glm::cross(T, e1);
            v = glm::dot(r.direction, Q) * inv_det;
            if(v < 0.f || u + v  > 1.f) {
                continue;
            }
            t = glm::dot(e2, Q) * inv_det;
            if(t > 0.00001 && t < t_smallest) { //ray intersection
                t_smallest = t;
                glm::vec3 n0 = mesh_->normals_[triangle_indices_[i + 0]];
                glm::vec3 n1 = mesh_->normals_[triangle_indices_[i + 1]];
                glm::vec3 n2 = mesh_->normals_[triangle_indices_[i + 2]];
                
                glm::vec3 n = (1 - u - v) * n0 + u * n1 + v * n2;
                id->t = t;
                id->normal = glm::normalize(n);
                id->material = mesh_->material();
                intersect = true;
            }
        }
        return intersect;
    }
    else
    {
        IntersectionData closest_id;
        IntersectionData id_tmp;
        closest_id.t = 1000000000;
        bool intersect = false;
        for (int i = 0; i < 8; ++i)
            if(children_[i]->gg_.intersect(r))
                if (children_[i]->intersect(&id_tmp, r))
                    if (id_tmp.t < closest_id.t)
                    {
                        closest_id = id_tmp;
                        intersect = true;
                    }
        if (intersect)
        {
            *id = closest_id;
            return true;
        }
    }
    return false;
}