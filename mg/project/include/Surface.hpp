/***************************************************************************************************
 * @file  Surface.hpp
 * @brief Declaration of the Surface class
 **************************************************************************************************/

#pragma once

#include <cstdint>
#include <vector>
#include "mesh/Mesh.hpp"
#include "AABB.hpp"
#include "Blob.hpp"

std::uint64_t pack_cell(std::int64_t x, std::int64_t y, std::int64_t z);
vector3<std::int64_t> lattice_coords(const vec3& p, const vec3& global_origin, float grid_size);

struct PointImplicit {
    PointImplicit() : point(0.0f), implicit(0.0f) {}

    PointImplicit(float x, float y, float z) : point(x, y, z), implicit(0.0f) {}

    explicit PointImplicit(const vec3& point) : point(point), implicit(0.0f) {}

    vec3 point;
    float implicit;
};

class Surface {
public:
    Surface() : root(nullptr) {}

    explicit Surface(Blob* root) : root(root) {}

    [[nodiscard]] float implicit(const vec3& point) const;

    // Resources used to implement marching cubes:
    // Marching Cubes video by Sebastian Lague
    //   https://www.youtube.com/watch?v=M3iI2l0ltbE
    // Article by Paul Bourke
    //   https://paulbourke.net/geometry/polygonise/
    [[nodiscard]] Mesh compute_mesh(std::vector<AABB>& aabbs);

    [[nodiscard]] float compute_min_radius() const;

    [[nodiscard]] std::vector<AABB> compute_AABBs() const;

    [[nodiscard]] static vec3 interpolate_edge(const PointImplicit& A, const PointImplicit& B);

    Blob* root;
};
