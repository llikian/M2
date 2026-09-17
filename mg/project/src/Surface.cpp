/***************************************************************************************************
 * @file  Surface.cpp
 * @brief Implementation of the Surface class
 **************************************************************************************************/

#include "Surface.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include "maths/functions.hpp"
#include "maths/geometry.hpp"
#include "utility/LifetimeLogger.hpp"
#include "marching_cube_tables.hpp"

std::uint64_t pack_cell(std::int64_t x, std::int64_t y, std::int64_t z) {
    constexpr unsigned int BITS_PER_AXIS = 21; // 64 = 21 * 3 + 1
    constexpr std::int64_t AXIS_OFFSET = 1L << (BITS_PER_AXIS - 1);

    constexpr auto biased = [](std::int64_t v) -> std::uint64_t {
        return static_cast<std::uint64_t>(v + AXIS_OFFSET) & ((1ULL << BITS_PER_AXIS) - 1);
    };

    return (biased(x) << (2 * BITS_PER_AXIS)) | (biased(y) << BITS_PER_AXIS) | biased(z);
}

vector3<std::int64_t> lattice_coords(const vec3& p, const vec3& global_origin, float grid_size) {
    return vector3<std::int64_t>(static_cast<std::int64_t>(std::floor((p.x - global_origin.x) / grid_size)),
                                 static_cast<std::int64_t>(std::floor((p.y - global_origin.y) / grid_size)),
                                 static_cast<std::int64_t>(std::floor((p.z - global_origin.z) / grid_size)));
}

[[nodiscard]] float Surface::implicit(const vec3& point) const {
    return THRESHOLD - root->potential(point);
}

[[nodiscard]] Mesh Surface::compute_mesh(std::vector<AABB>& aabbs) {
    if(root == nullptr) { throw std::runtime_error("Can't create the mesh for an empty implicit surface"); }

    LifetimeLogger lifetime_logger("Mesh creation took: ");

    std::size_t blobs_count = 0;
    root->traverse(blobs_count, aabbs);

    std::cout << "The scene contains " << blobs_count << " blobs.\n";
    std::cout << aabbs.size() << " AABBs\n";

    Mesh mesh;
    mesh.set_primitive(MeshPrimitive::TRIANGLES);
    mesh.enable_attribute(ATTRIBUTE_NORMAL);

    vec3 global_min = aabbs[0].min;
    for(std::size_t i = 1; i < aabbs.size(); ++i) {
        global_min = vec3(std::min(global_min.x, aabbs[i].min.x),
                          std::min(global_min.y, aabbs[i].min.y),
                          std::min(global_min.z, aabbs[i].min.z));
    }

    std::cout << "Using a grid size of " << GRID_SIZE << '\n';

    std::unordered_set<std::uint64_t> visited_cubes;
    std::unordered_map<std::uint64_t, PointImplicit> points;

    const auto get_point = [&](std::int64_t x, std::int64_t y, std::int64_t z) -> const PointImplicit& {
        std::uint64_t key = pack_cell(x, y, z);
        auto [ite, inserted] = points.try_emplace(key);

        if(inserted) {
            ite->second.point = global_min + GRID_SIZE * vec3(x, y, z);
            ite->second.implicit = implicit(ite->second.point);
        }

        return ite->second;
    };

    for(const AABB& aabb : aabbs) {
        vector3<std::int64_t> min_corner = lattice_coords(aabb.min, global_min, GRID_SIZE);
        vector3<std::int64_t> max_corner = lattice_coords(aabb.max, global_min, GRID_SIZE);

        for(std::int64_t x = min_corner.x; x < max_corner.x; ++x) {
            for(std::int64_t y = min_corner.y; y < max_corner.y; ++y) {
                for(std::int64_t z = min_corner.z; z < max_corner.z; ++z) {
                    std::uint64_t key = pack_cell(x, y, z);
                    if(!visited_cubes.insert(key).second) { continue; }

                    const PointImplicit& p0 = get_point(x, y, z);
                    const PointImplicit& p1 = get_point(x + 1, y, z);
                    const PointImplicit& p2 = get_point(x + 1, y, z + 1);
                    const PointImplicit& p3 = get_point(x, y, z + 1);
                    const PointImplicit& p4 = get_point(x, y + 1, z);
                    const PointImplicit& p5 = get_point(x + 1, y + 1, z);
                    const PointImplicit& p6 = get_point(x + 1, y + 1, z + 1);
                    const PointImplicit& p7 = get_point(x, y + 1, z + 1);

                    const PointImplicit* corners[8] = { &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7 };

                    int cube_index = 0;
                    if(corners[0]->implicit < 0.0f) { cube_index |= 1; }
                    if(corners[1]->implicit < 0.0f) { cube_index |= 2; }
                    if(corners[2]->implicit < 0.0f) { cube_index |= 4; }
                    if(corners[3]->implicit < 0.0f) { cube_index |= 8; }
                    if(corners[4]->implicit < 0.0f) { cube_index |= 16; }
                    if(corners[5]->implicit < 0.0f) { cube_index |= 32; }
                    if(corners[6]->implicit < 0.0f) { cube_index |= 64; }
                    if(corners[7]->implicit < 0.0f) { cube_index |= 128; }

                    for(int i = 0; mc_triangles_table[cube_index][i] != -1; i += 3) {
                        int a0 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i]][0];
                        int b0 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i]][1];
                        vec3 A = interpolate_edge(*corners[a0], *corners[b0]);

                        int a1 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 1]][0];
                        int b1 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 1]][1];
                        vec3 B = interpolate_edge(*corners[a1], *corners[b1]);

                        int a2 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 2]][0];
                        int b2 = mc_edge_to_vertex_table[mc_triangles_table[cube_index][i + 2]][1];
                        vec3 C = interpolate_edge(*corners[a2], *corners[b2]);

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
    // float radius = blobs[0]->get_radius();
    // for(std::size_t i = 1; i < blobs.size(); ++i) { radius = std::min(radius, blobs[i]->get_radius()); }
    // return radius;
}

[[nodiscard]] std::vector<AABB> Surface::compute_AABBs() const {
    // std::vector<AABB> aabbs;
    // aabbs.reserve(blobs.size());
    // for(Blob* blob : blobs) { aabbs.push_back(blob->compute_AABB()); }

    // std::vector<bool> should_stay(aabbs.size(), true);
    // for(std::size_t i = 0; i + 1 < aabbs.size(); ++i) {
    //     const AABB& A = aabbs[i];

    //     for(std::size_t j = i + 1; j < aabbs.size(); ++j) {
    //         const AABB& B = aabbs[j];

    //         if(A.is_point_inside(B.min) && A.is_point_inside(B.max)) {
    //             should_stay[j] = false;
    //         } else if(B.is_point_inside(A.min) && B.is_point_inside(A.max)) {
    //             should_stay[i] = false;
    //         }
    //     }
    // }
    // std::vector<AABB> final_aabbs;
    // final_aabbs.reserve(blobs.size());
    // for(std::size_t i = 0; i < aabbs.size(); ++i) {
    //     if(should_stay[i]) { final_aabbs.push_back(aabbs[i]); }
    // }

    // std::cout << "Removed " << aabbs.size() - final_aabbs.size() << " aabbs that were inside others." << '\n';

    // return final_aabbs;
}

[[nodiscard]] vec3 Surface::interpolate_edge(const PointImplicit& A, const PointImplicit& B) {
    float diff = B.implicit - A.implicit;
    if(std::abs(diff) < 1e-6) { return A.point; }
    return A.point + ((-A.implicit) / diff) * (B.point - A.point);
}
