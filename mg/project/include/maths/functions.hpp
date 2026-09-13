/***************************************************************************************************
 * @file  functions.hpp
 * @brief Declaration of maths functions
 **************************************************************************************************/

#pragma once

#include "vec3.hpp"

inline float pow2(float x) {
    return x * x;
}

inline float length2(const vec3& vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline vec3 min(const vec3& A, const vec3& B) {
    return vec3(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z));
}

inline vec3 max(const vec3& A, const vec3& B) {
    return vec3(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z));
}

vec3 hue_to_rgb(unsigned short hue);
