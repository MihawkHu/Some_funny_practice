#ifndef _TOOL_H
#define _TOOL_H
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class SpectralDistribution {
public:
    SpectralDistribution();
    ~SpectralDistribution(){};

    float norm() const;

    friend std::ostream& operator<<(
        std::ostream& os,
        const SpectralDistribution& sd);
    friend SpectralDistribution operator*(
        float f,
        const SpectralDistribution& sd);
    float& operator[](const int i);
    
    static const int N_WAVELENGTHS = 3;
    static const int MIN_WAVELENGTH = 300;
    static const int MAX_WAVELENGTH = 700;
    float data[N_WAVELENGTHS];
    
    SpectralDistribution operator+(const SpectralDistribution& sd) const;
    SpectralDistribution operator-(const SpectralDistribution& sd) const;
    SpectralDistribution operator^(const float& f) const;
    SpectralDistribution operator/(const float& f) const;
    SpectralDistribution operator*(const float& f) const;
    SpectralDistribution operator*(const SpectralDistribution& sd) const;
    SpectralDistribution operator+=(const SpectralDistribution& sd);
    SpectralDistribution operator-=(const SpectralDistribution& sd);
    SpectralDistribution operator*=(const SpectralDistribution& sd);
    SpectralDistribution operator/=(const float& f);
    SpectralDistribution operator*=(const float& f);
};

struct Material
{
    SpectralDistribution color_diffuse;
    SpectralDistribution color_specular;
    float reflectance;
    float specular_reflectance; 
    float transmissivity;
    float refraction_index;
    float diffuse_roughness;

    static Material air();
};

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
    Material material; 
    SpectralDistribution radiance;
    bool has_intersected;
};

struct Photon
{
    glm::vec3 position;
    glm::vec3 direction_in;
    SpectralDistribution delta_flux;
    static const float RADIUS;
};

struct KDTreeNode
{
    typedef double value_type;
    Photon p;
    size_t index;

    value_type operator[](size_t n) const
    {
        return p.position[n];
    }

    double distance( const KDTreeNode &node)
    {
        return glm::length(node.p.position - p.position);
    }
};

struct IntersectionData
{
    Material material; // Material of the object hit by the ray
    glm::vec3 normal; // Normal of the surface hit by the ray
    float t; // The distance the ray travelled before intersecting
};

struct LightSourceIntersectionData
{
    SpectralDistribution radiosity; // The radiosity of the light source [Watts/m^2]
    float area; // The area of the light source [m^2]
    glm::vec3 normal; // Normal of the surface hit by the ray
    float t; // The distance the ray travelled before intersecting
};

SpectralDistribution evaluatePerfectBRDF(
    SpectralDistribution albedo);
SpectralDistribution evaluateLambertianBRDF(
    glm::vec3 d1,
    glm::vec3 d2,
    glm::vec3 normal,
    SpectralDistribution albedo);
SpectralDistribution evaluateOrenNayarBRDF(
    glm::vec3 d1,
    glm::vec3 d2,
    glm::vec3 normal,
    SpectralDistribution albedo,
    float roughness);

#endif