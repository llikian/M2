/***************************************************************************************************
 * @file  Mesh.cpp
 * @brief Implementation of the Mesh class
 **************************************************************************************************/

#include "Mesh.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include "glad/glad.h"
#include "maths/geometry.hpp"

bool is_whitespace(const std::string& line) {
    for(unsigned char c : line) {
        if(!std::isspace(c)) { return false; }
    }

    return true;
}

std::ostream& operator<<(std::ostream& stream, const Mesh::Triangle& tri) {
    stream << "verts: (" << tri.a << ", " << tri.b << ", " << tri.c << ") ; ";
    return stream;
}

std::ostream& operator<(std::ostream& stream, const Mesh::Face& face) {
    stream << "faces: (" << face.face_a << ", " << face.face_b << ", " << face.face_c << ")\n";
    return stream;
}

Mesh::Vertex::Vertex(float x, float y, float z, unsigned int face) : position(x, y, z), normal(), face(face) {}

Mesh::Triangle::Triangle(unsigned int a, unsigned int b, unsigned int c) : a(a), b(b), c(c) {}

Mesh::Face::Face(unsigned int face_a, unsigned int face_b, unsigned int face_c)
    : face_a(face_a),
      face_b(face_b),
      face_c(face_c) {}

void Mesh::make_tetrahedron(vec3 top, vec3 A, vec3 B, vec3 C) {
    vertices.emplace_back(top.x, top.y, top.z, 0);
    vertices.emplace_back(A.x, A.y, A.z, 0);
    vertices.emplace_back(B.x, B.y, B.z, 1);
    vertices.emplace_back(C.x, C.y, C.z, 2);

    indices.emplace_back(0, 1, 2);
    faces.emplace_back(3, 1, 2);
    indices.emplace_back(0, 2, 3);
    faces.emplace_back(3, 2, 0);
    indices.emplace_back(0, 3, 1);
    faces.emplace_back(3, 0, 1);
    indices.emplace_back(1, 3, 2);
    faces.emplace_back(1, 0, 2);
}

void Mesh::make_square_pyramid(vec3 top, vec3 A, vec3 B, vec3 C, vec3 D) {
    vertices.emplace_back(top.x, top.y, top.z, 0); // 0
    vertices.emplace_back(A.x, A.y, A.z, 0);       // 1
    vertices.emplace_back(B.x, B.y, B.z, 0);       // 2
    vertices.emplace_back(C.x, C.y, C.z, 1);       // 3
    vertices.emplace_back(D.x, D.y, D.z, 1);       // 4

    indices.emplace_back(0, 1, 2); // 0
    faces.emplace_back(4, 1, 3);
    indices.emplace_back(0, 2, 3); // 1
    faces.emplace_back(4, 2, 0);
    indices.emplace_back(0, 3, 4); // 2
    faces.emplace_back(5, 3, 1);
    indices.emplace_back(0, 4, 1); // 3
    faces.emplace_back(5, 0, 2);
    indices.emplace_back(1, 3, 2); // 4
    faces.emplace_back(1, 0, 5);
    indices.emplace_back(1, 4, 3); // 5
    faces.emplace_back(2, 4, 3);
}

void Mesh::make_bounding_box_2D(float min_x, float min_y, float max_x, float max_y) {
    vertices.emplace_back(min_x, max_y, 0.0f, 0); // 0 - top left
    vertices.emplace_back(min_x, min_y, 0.0f, 0); // 1 - bottom left
    vertices.emplace_back(max_x, min_y, 0.0f, 0); // 2 - bottom right
    vertices.emplace_back(max_x, max_y, 0.0f, 1); // 3 - top right
    vertices.emplace_back(0.5f * (min_x + max_x),
                          0.5f * (min_y + max_y),
                          std::numeric_limits<float>::infinity(),
                          2); // 4 - artificial infinite vertex

    indices.emplace_back(0, 1, 2); // 0 - triangle
    faces.emplace_back(4, 1, 5);
    indices.emplace_back(0, 2, 3); // 1 - triangle
    faces.emplace_back(3, 2, 0);
    indices.emplace_back(4, 0, 3); // 2 - artificial triangle
    faces.emplace_back(1, 3, 5);
    indices.emplace_back(4, 3, 2); // 3 - artificial triangle
    faces.emplace_back(1, 4, 2);
    indices.emplace_back(4, 1, 2); // 4 - artificial triangle
    faces.emplace_back(0, 3, 5);
    indices.emplace_back(4, 1, 0); // 5 - artificial triangle
    faces.emplace_back(0, 2, 4);
}

void Mesh::bind_buffers() {
    /* VAO */
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    /* VBO */
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    /* Vertex Attributes */
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(sizeof(vec3)));
    glEnableVertexAttribArray(1);

    /* Indices & EBO */
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Triangle), indices.data(), GL_STATIC_DRAW);
}

void Mesh::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr);
}

void Mesh::compute_normals() {
    for(const Triangle& tri : indices) {
        const vec3& A = vertices[tri.a].position;
        const vec3& B = vertices[tri.b].position;
        const vec3& C = vertices[tri.c].position;

        vec3 normal = normalize(cross(B - A, C - A));

        vertices[tri.a].normal += normal;
        vertices[tri.b].normal += normal;
        vertices[tri.c].normal += normal;
    }

    for(Vertex& vertex : vertices) { vertex.normal = normalize(vertex.normal); }
}

void Mesh::save_to_off(const std::filesystem::path& path) {
    std::ofstream out(path);

    out << "OFF\n";
    out << vertices.size() << ' ' << indices.size() << ' ' << 0 << '\n';

    out << '\n';
    for(const Vertex& v : vertices) { out << v.position.x << ' ' << v.position.y << ' ' << v.position.z << '\n'; }

    out << '\n';
    for(const Triangle& t : indices) { out << 3 << ' ' << t.a << ' ' << t.b << ' ' << t.c << '\n'; }
}

void Mesh::load_from_off(const std::filesystem::path& path) {
    std::ifstream file(path);
    if(!file.is_open()) { throw std::runtime_error("Couldn't load .off file.\n"); }

    std::string line;
    std::string buffer;

    unsigned int vertex_count;
    unsigned int face_count;

    file >> buffer;
    if(buffer == "off" || buffer == "OFF") {
        file >> vertex_count >> face_count >> buffer;
    } else {
        vertex_count = std::stoi(buffer);
        file >> face_count;
    }

    std::cout << vertex_count << ' ' << face_count << '\n';

    vertices.reserve(vertex_count + 1);
    indices.reserve(face_count);
    faces.reserve(face_count);

    std::vector<unsigned int> face_indices;
    face_indices.resize(5);

    for(unsigned int i = 0; i < vertex_count; ++i) {
        do { std::getline(file, line); } while(line[0] == '#' || is_whitespace(line));
        std::stringstream stream(line);

        float x, y, z;
        stream >> x >> y >> z;
        vertices.emplace_back(x, y, z, -1);
    }

    using pair_uu = std::pair<unsigned int, unsigned int>;
    // Maps (first vertex index, second vertex index) to (face index, edge index in face)
    std::map<pair_uu, pair_uu> edges;

    auto find_opposite_edge = [&](unsigned int a, unsigned int b, unsigned int face, unsigned int local_index) {
        auto ite = edges.find(pair_uu(b, a));

        if(ite == edges.end()) {
            edges.emplace(pair_uu(a, b), pair_uu(face, local_index));
        } else {
            (&faces[face].face_a)[local_index] = ite->second.first;
            (&faces[ite->second.first].face_a)[ite->second.second] = face;

            edges.erase(ite);
        }
    };

    for(unsigned int i = 0; i < face_count; ++i) {
        do { std::getline(file, line); } while(line[0] == '#' || is_whitespace(line));
        std::stringstream stream(line);

        unsigned int vertices_in_face;
        stream >> vertices_in_face;

        if(vertices_in_face > face_indices.size()) { face_indices.resize(vertices_in_face); }

        for(unsigned int i = 0; i < vertices_in_face; ++i) { stream >> face_indices[i]; }

        for(unsigned int i = 0; i + 2 < vertices_in_face; ++i) {
            unsigned int a = face_indices[0];
            unsigned int b = face_indices[i + 1];
            unsigned int c = face_indices[i + 2];
            unsigned int face = indices.size();

            indices.emplace_back(a, b, c);
            faces.emplace_back(-1, -1, -1);

            vertices[a].face = vertices[b].face = vertices[c].face = face;

            find_opposite_edge(b, c, face, 0);
            find_opposite_edge(c, a, face, 1);
            find_opposite_edge(a, b, face, 2);
        }
    }

    unsigned int triangle_count = indices.size();

    if(!edges.empty()) {
        throw std::runtime_error("Unhandled case : There's an edge.");

        indices.reserve(indices.size() + edges.size());
        faces.reserve(indices.size() + edges.size());

        unsigned int artificial_vertex_id = vertices.size();
        vertices.emplace_back(INFINITY, INFINITY, INFINITY, triangle_count);

        std::cout << "\nThis mesh has an edge\n";
        std::cout << "Triangles before edge: " << triangle_count << '\n';

        std::vector<std::pair<pair_uu, pair_uu>> boundary;
        boundary.reserve(edges.size());
        for(const auto& e : edges) { boundary.emplace_back(e); }

        for(const auto& e : boundary) {
            const auto [a, b] = e.first;
            const auto [face, local_index] = e.second;

            indices.emplace_back(artificial_vertex_id, b, a);
            faces.emplace_back(face, -1, -1);

            find_opposite_edge(a, artificial_vertex_id, face, 1);
            find_opposite_edge(artificial_vertex_id, b, face, 2);

            (&faces[face].face_a)[local_index] = faces.size() - 1;
        }
    }

    if(!check()) { throw std::runtime_error("Mesh check failed"); }
}

bool Mesh::check() const {
    for(unsigned int i = 0; i < vertices.size(); ++i) {
        const Triangle& tri = indices[vertices[i].face];
        if(tri.a != i && tri.b != i && tri.c != i) {
            std::cerr << "A vertex had the id of a face it wasn't a part of.\n";
            return false;
        }
    }

    for(unsigned int i = 0; i < faces.size(); ++i) {
        const Triangle& tri = indices[i];
        const Face& face = faces[i];
        const Triangle& tri_a = indices[face.face_a]; // find opposite of b, c (c, b)
        const Triangle& tri_b = indices[face.face_b]; // find opposite of c, a (a, c)
        const Triangle& tri_c = indices[face.face_c]; // find opposite of a, b (b, a)

        if(!(tri_a.a == tri.c && tri_a.b == tri.b) && //
           !(tri_a.b == tri.c && tri_a.c == tri.b) && //
           !(tri_a.c == tri.c && tri_a.a == tri.b)) {
            std::cerr << "A triangle doesn't have the proper opposite face for a vertex.\n";
            std::cerr << "tri   (" << i << "): " << tri << "tri_a (" << face.face_a << "): " << tri_a << '\n';
            return false;
        }

        if(!(tri_b.a == tri.a && tri_b.b == tri.c) && //
           !(tri_b.b == tri.a && tri_b.c == tri.c) && //
           !(tri_b.c == tri.a && tri_b.a == tri.c)) {
            std::cerr << "A triangle doesn't have the proper opposite face for a vertex.\n";
            std::cerr << "tri   (" << i << "): " << tri << "tri_b (" << face.face_b << "): " << tri_b << '\n';
            return false;
        }

        if(!(tri_c.a == tri.b && tri_c.b == tri.a) && //
           !(tri_c.b == tri.b && tri_c.c == tri.a) && //
           !(tri_c.c == tri.b && tri_c.a == tri.a)) {
            std::cerr << "A triangle doesn't have the proper opposite face for a vertex.\n";
            std::cerr << "tri   (" << i << "): " << tri << "tri_c (" << face.face_c << "): " << tri_c << '\n';
            return false;
        }
    }

    return true;
};
