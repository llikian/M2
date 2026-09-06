/***************************************************************************************************
 * @file  main.cpp
 * @brief Contains the main program of the project
 **************************************************************************************************/

#include <cmath>
#include <iostream>
#include "MeshJC.hpp"
#include "MeshRC.hpp"

int main() {
    try {
        /* Tetrahedron */ {
            Point top(0.0f, 2.0f, 0.0f);
            Point A(0.0f, 0.0f, 2.0f);
            Point B(2.0f * std::cos(7.0f * M_PI / 6.0f), 0.0f, 2.0f * std::sin(7.0f * M_PI / 6.0f));
            Point C(2.0f * std::cos(11.0f * M_PI / 6.0f), 0.0f, 2.0f * std::sin(11.0f * M_PI / 6.0f));

            MeshJC jc_tetrahedron;
            MeshRC rc_tetrahedron;
            jc_tetrahedron.make_tetrahedron(top, A, B, C);
            rc_tetrahedron.make_tetrahedron(top, A, B, C);
            rc_tetrahedron.save_to_off("data/tetrahedron.off");

            A = Point(-1.0f, 0.0f, 1.0f);
            B = Point(-1.0f, 0.0f, -1.0f);
            C = Point(1.0f, 0.0f, -1.0f);
            Point D(1.0f, 0.0f, 1.0f);

            MeshJC jc_sqr_pyramid;
            MeshRC rc_sqr_pyramid;
            jc_sqr_pyramid.make_square_pyramid(top, A, B, C, D);
            rc_sqr_pyramid.make_square_pyramid(top, A, B, C, D);
            rc_sqr_pyramid.save_to_off("data/square_pyramid.off");

            float min_x = -1.0f, min_y = -1.0f;
            float max_x = 1.0f, max_y = 1.0f;

            MeshJC jc_bounding_box_2D;
            MeshRC rc_bounding_box_2D;
            jc_bounding_box_2D.make_bounding_box_2D(min_x, min_y, max_x, max_y);
            rc_bounding_box_2D.make_bounding_box_2D(min_x, min_y, max_x, max_y);
            rc_bounding_box_2D.save_to_off("data/bounding_box_2D.off");

            MeshRC apple;
            apple.load_from_off("data/pear.off");
            // apple.load_from_off("data/r2.off");
            apple.save_to_off("data/out.off");
        }

    } catch(const std::exception& exception) {
        std::cerr << "ERROR : " << exception.what() << '\n';
        return -1;
    }

    return 0;
}
