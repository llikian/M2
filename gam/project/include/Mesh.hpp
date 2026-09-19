/***************************************************************************************************
 * @file  Mesh.hpp
 * @brief Declaration of the Mesh class
 **************************************************************************************************/

#pragma once

#include <filesystem>
#include <vector>
#include "maths/vec3.hpp"

bool is_whitespace(const std::string& line);

class Mesh {
public:
    struct Vertex {
        Vertex(float x, float y, float z, unsigned int face);

        vec3 position;
        vec3 normal;
        unsigned int face;
    };

    struct Triangle {
        Triangle(unsigned int a, unsigned int b, unsigned int c);

        unsigned int operator[](unsigned int index) const { return (&a)[index]; }

        unsigned int& operator[](unsigned int index) { return (&a)[index]; }

        unsigned int a, b, c;
    };

    struct Face {
        Face(unsigned int face_a, unsigned int face_b, unsigned int face_c);

        unsigned int operator[](unsigned int index) const { return (&alpha)[index]; }

        unsigned int& operator[](unsigned int index) { return (&alpha)[index]; }

        unsigned int alpha, beta, gamma;
    };

    void make_tetrahedron(vec3 top, vec3 A, vec3 B, vec3 C);

    void make_square_pyramid(vec3 top, vec3 A, vec3 B, vec3 C, vec3 D);

    void make_bounding_box_2D(float min_x, float min_y, float max_x, float max_y);

    void bind_buffers();
    void draw();
    void compute_normals();

    void save_to_off(const std::filesystem::path& path);
    void load_from_off(const std::filesystem::path& path);

    void triangle_split(unsigned int face, const vec3& point);
    void edge_split(unsigned int face, unsigned int vertex, const vec3& point);

    bool check() const;

    unsigned int get_local_index(unsigned int vertex, unsigned int face);

    void draw_imgui_table() const;

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Face> faces;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};
