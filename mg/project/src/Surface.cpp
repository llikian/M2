/***************************************************************************************************
 * @file  Surface.cpp
 * @brief Implementation of the Surface class
 **************************************************************************************************/

#include "Surface.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include "maths/functions.hpp"
#include "maths/geometry.hpp"
#include "utility/LifetimeLogger.hpp"
#include "marching_cube_tables.hpp"

bool operator<(const AABB& left, const AABB& right) {
    // TODO: check if this correct
    return left.second.x < right.first.x || left.second.y < right.first.y || left.second.z < right.first.z;
}

Surface::~Surface() {
    for(auto& blob : blobs) { delete blob; }
}

[[nodiscard]] float Surface::potentials_sum(const vec3& point) const {
    float total = 0.0f;
    for(const auto& blob : blobs) { total += blob->potential(point); }
    return total;
}

[[nodiscard]] float Surface::implicit(const vec3& point) const {
    return threshold - potentials_sum(point);
}

[[nodiscard]] Mesh Surface::compute_mesh() {
    if(blobs.empty()) { throw std::runtime_error("Can't create the mesh for an empty implicit surface"); }

    LifetimeLogger lifetime_logger("Mesh creation took: ");

    Mesh mesh;
    mesh.set_primitive(MeshPrimitive::TRIANGLES);
    mesh.enable_attribute(ATTRIBUTE_NORMAL);

    std::vector<AABB> aabbs = compute_AABBs();
    int cubes_in_smallest_blob = 8;
    float grid_size = compute_min_radius() / static_cast<float>(cubes_in_smallest_blob);

    std::vector<PointImplicit> points;

    for(const AABB& aabb : aabbs) {
        vec3 aabb_span = aabb.second - aabb.first;
        // Use a region slightly larger than the bounding box of the surface
        vec3 margin = 0.05f * aabb_span;
        vec3 corner = aabb.first - margin;
        aabb_span = aabb.second + margin - corner;
        corner = vec3(std::floor(corner.x), std::floor(corner.y), std::floor(corner.z));

        vector3<std::size_t> span(aabb_span.x / grid_size, aabb_span.y / grid_size, aabb_span.z / grid_size);

        points.resize(span.x * span.y * span.z);

        for(std::size_t x = 0; x < span.x; ++x) {
            for(std::size_t y = 0; y < span.y; ++y) {
                for(std::size_t z = 0; z < span.z; ++z) {
                    std::size_t index = x + (y + z * span.y) * span.x;
                    points[index].point = corner + grid_size * vec3(x, y, z);
                    points[index].implicit = implicit(points[index].point);
                }
            }
        }

        for(std::size_t x = 0; x + 1 < span.x; ++x) {
            for(std::size_t y = 0; y + 1 < span.y; ++y) {
                for(std::size_t z = 0; z + 1 < span.z; ++z) {
                    std::size_t indices[8] {
                        x + (y + z * span.y) * span.x,
                        (x + 1) + (y + z * span.y) * span.x,
                        (x + 1) + (y + (z + 1) * span.y) * span.x,
                        x + (y + (z + 1) * span.y) * span.x,
                        x + ((y + 1) + z * span.y) * span.x,
                        (x + 1) + ((y + 1) + z * span.y) * span.x,
                        (x + 1) + ((y + 1) + (z + 1) * span.y) * span.x,
                        x + ((y + 1) + (z + 1) * span.y) * span.x,

                    };

                    int cube_index = 0;
                    if(points[indices[0]].implicit < 0.0f) { cube_index |= 1; }
                    if(points[indices[1]].implicit < 0.0f) { cube_index |= 2; }
                    if(points[indices[2]].implicit < 0.0f) { cube_index |= 4; }
                    if(points[indices[3]].implicit < 0.0f) { cube_index |= 8; }
                    if(points[indices[4]].implicit < 0.0f) { cube_index |= 16; }
                    if(points[indices[5]].implicit < 0.0f) { cube_index |= 32; }
                    if(points[indices[6]].implicit < 0.0f) { cube_index |= 64; }
                    if(points[indices[7]].implicit < 0.0f) { cube_index |= 128; }

                    for(int i = 0; mc_triangles_table[cube_index][i] != -1; i += 3) {
                        int a0 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i]][0];
                        int b0 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i]][1];
                        vec3 A = interpolate_edge(points[indices[a0]], points[indices[b0]]);

                        int a1 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 1]][0];
                        int b1 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 1]][1];
                        vec3 B = interpolate_edge(points[indices[a1]], points[indices[b1]]);

                        int a2 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 2]][0];
                        int b2 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 2]][1];
                        vec3 C = interpolate_edge(points[indices[a2]], points[indices[b2]]);

                        vec3 normal = normalize(cross(B - A, C - A));

                        mesh.add_vertex(A, normal);
                        mesh.add_vertex(B, normal);
                        mesh.add_vertex(C, normal);
                    }
                }
            }
        }
    }

    mesh.bind_buffers();

    std::cout << "The mesh has " << mesh.get_vertices_amount() << " vertices.\n";

    return mesh;
}

[[nodiscard]] float Surface::compute_min_radius() const {
    float radius = blobs[0]->get_radius();
    for(std::size_t i = 1; i < blobs.size(); ++i) { radius = std::min(radius, blobs[i]->get_radius()); }
    return radius;
}

[[nodiscard]] std::vector<AABB> Surface::compute_AABBs() const {
    std::vector<AABB> aabbs;
    aabbs.reserve(blobs.size());
    for(Blob* blob : blobs) { aabbs.push_back(blob->compute_AABB()); }
    return aabbs;
}

[[nodiscard]] vec3 Surface::interpolate_edge(const PointImplicit& A, const PointImplicit& B) {
    float diff = B.implicit - A.implicit;
    if(std::abs(diff) < 1e-6) { return A.point; }
    return A.point + ((-A.implicit) / diff) * (B.point - A.point);
}
