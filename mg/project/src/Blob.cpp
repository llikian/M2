/***************************************************************************************************
 * @file  Blob.cpp
 * @brief Implementation of the Blob class
 **************************************************************************************************/

#include "Blob.hpp"

#include "maths/functions.hpp"
#include "maths/geometry.hpp"

float attenuation_wyvill(float distance_sqr, int n) {
    float base = 1.0f - distance_sqr;
    float result = 1.0f;

    for(int i = 0; i < n; ++i) { result *= base; }

    return result;
}

float attenuation_polynomial(float distance_sqr) {
    return pow2(1.0f - distance_sqr) * (1.0f - (4.0f / 9.0f) * distance_sqr);
}

[[nodiscard]] float SphereBlob::potential(const vec3& point) const {
    float distance_sqr = length2(center - point) / (radius * radius);
    if(distance_sqr >= 1.0f) { return 0.0f; }
    return attenuation_wyvill(distance_sqr, 2);
}

[[nodiscard]] std::pair<vec3, vec3> SphereBlob::compute_AABB() const {
    return { center - radius, center + radius };
}

[[nodiscard]] float CapsuleBlob::potential(const vec3& point) const {
    vec3 AB = B - A;
    float dist_AB_sqr = length2(AB);

    float t = (dist_AB_sqr > 0.0f) ? dot(point - A, AB) / dist_AB_sqr : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    float distance_sqr = length2(point - (A + t * AB)) / (radius * radius);
    if(distance_sqr >= 1.0f) { return 0.0f; }
    return attenuation_wyvill(distance_sqr, 2);
}

[[nodiscard]] std::pair<vec3, vec3> CapsuleBlob::compute_AABB() const {
    return { min(A, B) - radius, max(A, B) + radius };
}
