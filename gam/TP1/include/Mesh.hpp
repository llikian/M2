/***************************************************************************************************
 * @file  Mesh.hpp
 * @brief Declaration of the Mesh class
 **************************************************************************************************/

#pragma once

#include <filesystem>
#include <vector>
#include "Point.hpp"

bool is_whitespace(const std::string& line);

class Mesh {
public:
    struct Vertex : Point {
        Vertex(float x, float y, float z, unsigned int face);

        unsigned int face;
    };

    struct Triangle {
        Triangle(unsigned int a,
                 unsigned int b,
                 unsigned int c,
                 unsigned int face_a,
                 unsigned int face_b,
                 unsigned int face_c);

        unsigned int a, b, c;
        unsigned int face_a, face_b, face_c;
    };

    void make_tetrahedron(Point top, Point A, Point B, Point C);

    void make_square_pyramid(Point top, Point A, Point B, Point C, Point D);

    void make_bounding_box_2D(float min_x, float min_y, float max_x, float max_y);

    void save_to_off(const std::filesystem::path& path);

    void load_from_off(const std::filesystem::path& path);

    bool check() const;

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};
