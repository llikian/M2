/***************************************************************************************************
 * @file  Surface.hpp
 * @brief Declaration of the Surface class
 **************************************************************************************************/

#pragma once

#include <vector>
#include "mesh/Mesh.hpp"
#include "Blob.hpp"

using AABB = std::pair<vec3, vec3>;

bool operator<(const AABB& left, const AABB& right);

struct PointImplicit {
    PointImplicit() : point(0.0f), implicit(0.0f) {}

    PointImplicit(float x, float y, float z) : point(x, y, z), implicit(0.0f) {}

    explicit PointImplicit(const vec3& point) : point(point), implicit(0.0f) {}

    vec3 point;
    float implicit;
};

class Surface {
public:
    explicit Surface(float threshold) : threshold(threshold) {}

    ~Surface();

    template <typename T, typename... Args>
    T* add(Args&&... args) {
        blobs.push_back(new T(std::forward<Args>(args)...));
        return static_cast<T*>(blobs.back());
    }

    [[nodiscard]] float potentials_sum(const vec3& point) const;

    [[nodiscard]] float implicit(const vec3& point) const;

    // Resources used to implement marching cubes:
    // Marching Cubes video by Sebastian Lague
    //   https://www.youtube.com/watch?v=M3iI2l0ltbE
    // Article by Paul Bourke
    //   https://paulbourke.net/geometry/polygonise/
    [[nodiscard]] Mesh compute_mesh();

    [[nodiscard]] float compute_min_radius() const;

    [[nodiscard]] std::vector<AABB> compute_AABBs() const;

    [[nodiscard]] static vec3 interpolate_edge(const PointImplicit& A, const PointImplicit& B);

    float threshold;

private:
    std::vector<Blob*> blobs;
    std::vector<AABB> aabbs;
};
