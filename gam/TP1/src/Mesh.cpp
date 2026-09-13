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

bool is_whitespace(const std::string& line) {
    for(unsigned char c : line) {
        if(!std::isspace(c)) { return false; }
    }

    return true;
}

std::ostream& operator<<(std::ostream& stream, const Mesh::Triangle& tri) {
    stream << "verts: (" << tri.a << ", " << tri.b << ", " << tri.c << ") ; ";
    stream << "faces: (" << tri.face_a << ", " << tri.face_b << ", " << tri.face_c << ")\n";
    return stream;
}

Mesh::Vertex::Vertex(float x, float y, float z, unsigned int face) : Point(x, y, z), face(face) {}

Mesh::Triangle::Triangle(unsigned int a,
                         unsigned int b,
                         unsigned int c,
                         unsigned int face_a,
                         unsigned int face_b,
                         unsigned int face_c)
    : a(a),
      b(b),
      c(c),
      face_a(face_a),
      face_b(face_b),
      face_c(face_c) {}

void Mesh::make_tetrahedron(Point top, Point A, Point B, Point C) {
    vertices.emplace_back(top.x, top.y, top.z, 0);
    vertices.emplace_back(A.x, A.y, A.z, 0);
    vertices.emplace_back(B.x, B.y, B.z, 1);
    vertices.emplace_back(C.x, C.y, C.z, 2);

    triangles.emplace_back(0, 1, 2, 3, 1, 2);
    triangles.emplace_back(0, 2, 3, 3, 2, 0);
    triangles.emplace_back(0, 3, 1, 3, 0, 1);
    triangles.emplace_back(1, 3, 2, 1, 0, 2);
}

void Mesh::make_square_pyramid(Point top, Point A, Point B, Point C, Point D) {
    vertices.emplace_back(top.x, top.y, top.z, 0); // 0
    vertices.emplace_back(A.x, A.y, A.z, 0);       // 1
    vertices.emplace_back(B.x, B.y, B.z, 0);       // 2
    vertices.emplace_back(C.x, C.y, C.z, 1);       // 3
    vertices.emplace_back(D.x, D.y, D.z, 1);       // 4

    triangles.emplace_back(0, 1, 2, 4, 1, 3); // 0
    triangles.emplace_back(0, 2, 3, 4, 2, 0); // 1
    triangles.emplace_back(0, 3, 4, 5, 3, 1); // 2
    triangles.emplace_back(0, 4, 1, 5, 0, 2); // 3
    triangles.emplace_back(1, 3, 2, 1, 0, 5); // 4
    triangles.emplace_back(1, 4, 3, 2, 4, 3); // 5
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

    triangles.emplace_back(0, 1, 2, 4, 1, 5); // 0 - triangle
    triangles.emplace_back(0, 2, 3, 3, 2, 0); // 1 - triangle
    triangles.emplace_back(4, 0, 3, 1, 3, 5); // 2 - artificial triangle
    triangles.emplace_back(4, 3, 2, 1, 4, 2); // 3 - artificial triangle
    triangles.emplace_back(4, 1, 2, 0, 3, 5); // 4 - artificial triangle
    triangles.emplace_back(4, 1, 0, 0, 2, 4); // 5 - artificial triangle
}

void Mesh::save_to_off(const std::filesystem::path& path) {
    std::ofstream out(path);

    out << "OFF\n";
    out << vertices.size() << ' ' << triangles.size() << ' ' << 0 << '\n';

    out << '\n';
    for(const Vertex& v : vertices) { out << v.x << ' ' << v.y << ' ' << v.z << '\n'; }

    out << '\n';
    for(const Triangle& t : triangles) { out << 3 << ' ' << t.a << ' ' << t.b << ' ' << t.c << '\n'; }
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
    triangles.reserve(face_count);

    std::vector<unsigned int> indices;
    indices.resize(5);

    for(unsigned int i = 0; i < vertex_count; ++i) {
        do { std::getline(file, line); } while(line[0] == '#' || is_whitespace(line));
        std::stringstream stream(line);

        float x, y, z;
        stream >> x >> y >> z;
        vertices.emplace_back(x, z, y, -1);
    }

    using pair_uu = std::pair<unsigned int, unsigned int>;
    // Maps (first vertex index, second vertex index) to (face index, edge index in face)
    std::map<pair_uu, pair_uu> edges;

    auto find_opposite_edge = [&](unsigned int a, unsigned int b, unsigned int face, unsigned int local_index) {
        auto ite = edges.find(pair_uu(b, a));

        if(ite == edges.end()) {
            edges.emplace(pair_uu(a, b), pair_uu(face, local_index));
        } else {
            (&triangles[face].face_a)[local_index] = ite->second.first;
            (&triangles[ite->second.first].face_a)[ite->second.second] = face;

            edges.erase(ite);
        }
    };

    for(unsigned int i = 0; i < face_count; ++i) {
        do { std::getline(file, line); } while(line[0] == '#' || is_whitespace(line));
        std::stringstream stream(line);

        unsigned int vertices_in_face;
        stream >> vertices_in_face;

        if(vertices_in_face > indices.size()) { indices.resize(vertices_in_face); }

        for(unsigned int i = 0; i < vertices_in_face; ++i) { stream >> indices[i]; }

        for(unsigned int i = 0; i + 2 < vertices_in_face; ++i) {
            unsigned int a = indices[0];
            unsigned int b = indices[i + 1];
            unsigned int c = indices[i + 2];
            unsigned int face = triangles.size();

            triangles.emplace_back(a, b, c, -1, -1, -1);

            vertices[a].face = vertices[b].face = vertices[c].face = face;

            find_opposite_edge(b, c, face, 0);
            find_opposite_edge(c, a, face, 1);
            find_opposite_edge(a, b, face, 2);
        }
    }

    unsigned int triangle_count = triangles.size();

    if(!edges.empty()) {
        throw std::runtime_error("Unhandled case : There's an edge.");

        triangles.reserve(triangles.size() + edges.size());

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

            triangles.emplace_back(artificial_vertex_id, b, a, face, -1, -1);

            find_opposite_edge(a, artificial_vertex_id, face, 1);
            find_opposite_edge(artificial_vertex_id, b, face, 2);

            (&triangles[face].face_a)[local_index] = triangles.size() - 1;
        }
    }

    if(!check()) { throw std::runtime_error("Mesh check failed"); }
}

bool Mesh::check() const {
    for(unsigned int i = 0; i < vertices.size(); ++i) {
        const Triangle& tri = triangles[vertices[i].face];
        if(tri.a != i && tri.b != i && tri.c != i) {
            std::cerr << "A vertex had the id of a face it wasn't a part of.\n";
            return false;
        }
    }

    for(unsigned int i = 0; i < triangles.size(); ++i) {
        const Triangle& tri = triangles[i];
        const Triangle& tri_a = triangles[tri.face_a]; // find opposite of b, c (c, b)
        const Triangle& tri_b = triangles[tri.face_b]; // find opposite of c, a (a, c)
        const Triangle& tri_c = triangles[tri.face_c]; // find opposite of a, b (b, a)

        if(!(tri_a.a == tri.c && tri_a.b == tri.b) && //
           !(tri_a.b == tri.c && tri_a.c == tri.b) && //
           !(tri_a.c == tri.c && tri_a.a == tri.b)) {
            std::cerr << "A triangle doesn't have the proper opposite face for a vertex.\n";
            std::cerr << "tri   (" << i << "): " << tri << "tri_a (" << tri.face_a << "): " << tri_a << '\n';
            return false;
        }

        if(!(tri_b.a == tri.a && tri_b.b == tri.c) && //
           !(tri_b.b == tri.a && tri_b.c == tri.c) && //
           !(tri_b.c == tri.a && tri_b.a == tri.c)) {
            std::cerr << "A triangle doesn't have the proper opposite face for a vertex.\n";
            std::cerr << "tri   (" << i << "): " << tri << "tri_b (" << tri.face_b << "): " << tri_b << '\n';
            return false;
        }

        if(!(tri_c.a == tri.b && tri_c.b == tri.a) && //
           !(tri_c.b == tri.b && tri_c.c == tri.a) && //
           !(tri_c.c == tri.b && tri_c.a == tri.a)) {
            std::cerr << "A triangle doesn't have the proper opposite face for a vertex.\n";
            std::cerr << "tri   (" << i << "): " << tri << "tri_c (" << tri.face_c << "): " << tri_c << '\n';
            return false;
        }
    }

    return true;
};
