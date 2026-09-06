/***************************************************************************************************
 * @file  MeshJC.hpp
 * @brief Declaration of the MeshJC class
 **************************************************************************************************/

#pragma once

#include <vector>
#include "Point.hpp"

class MeshJC {
public:
    struct Vertex : Point {
        Vertex(float x, float y, float z) : Point(x, y, z) {}
    };

    struct Triangle {
        unsigned int a, b, c;
    };

    void make_tetrahedron(Point top, Point A, Point B, Point C) {
        vertices.emplace_back(top.x, top.y, top.z); // 0
        vertices.emplace_back(A.x, A.y, A.z);       // 1
        vertices.emplace_back(B.x, B.y, B.z);       // 2
        vertices.emplace_back(C.x, C.y, C.z);       // 3

        triangles.emplace_back(0, 1, 2); // 0
        triangles.emplace_back(0, 2, 3); // 1
        triangles.emplace_back(0, 3, 1); // 2
        triangles.emplace_back(1, 3, 2); // 3
    }

    void make_square_pyramid(Point top, Point A, Point B, Point C, Point D) {
        vertices.emplace_back(top.x, top.y, top.z); // 0
        vertices.emplace_back(A.x, A.y, A.z);       // 1
        vertices.emplace_back(B.x, B.y, B.z);       // 2
        vertices.emplace_back(C.x, C.y, C.z);       // 3
        vertices.emplace_back(D.x, D.y, D.z);       // 4

        triangles.emplace_back(0, 1, 2); // 0
        triangles.emplace_back(0, 2, 3); // 1
        triangles.emplace_back(0, 3, 4); // 2
        triangles.emplace_back(0, 4, 1); // 3
        triangles.emplace_back(1, 3, 2); // 4
        triangles.emplace_back(1, 4, 3); // 5
    }

    void make_bounding_box_2D(float min_x, float min_y, float max_x, float max_y) {
        vertices.emplace_back(min_x, max_y, 0.0f); // 0 - top left
        vertices.emplace_back(min_x, min_y, 0.0f); // 1 - bottom left
        vertices.emplace_back(max_x, min_y, 0.0f); // 2 - bottom right
        vertices.emplace_back(max_x, max_y, 0.0f); // 3 - top right
        vertices.emplace_back(0.5f * (min_x + max_x),
                              0.5f * (min_y + max_y),
                              std::numeric_limits<float>::infinity()); // 4 - artificial infinite vertex

        triangles.emplace_back(0, 1, 2); // 0 - triangle
        triangles.emplace_back(0, 2, 3); // 1 - triangle
        triangles.emplace_back(4, 0, 3); // 2 - artificial triangle
        triangles.emplace_back(4, 3, 2); // 3 - artificial triangle
        triangles.emplace_back(4, 1, 2); // 4 - artificial triangle
        triangles.emplace_back(4, 1, 0); // 5 - artificial triangle
    };

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};
