/***************************************************************************************************
 * @file  Blob.hpp
 * @brief Declaration of the Blob class
 **************************************************************************************************/

#pragma once

#include "maths/vec3.hpp"

float attenuation_wyvill(float distance_sqr, int n);
float attenuation_polynomial(float distance_sqr);

struct Blob {
    virtual ~Blob() = default;

    [[nodiscard]] virtual float potential(const vec3& point) const = 0;

    [[nodiscard]] virtual std::pair<vec3, vec3> compute_AABB() const = 0;

    [[nodiscard]] virtual float get_radius() const { return std::numeric_limits<float>::infinity(); }
};

struct SphereBlob : Blob {
    SphereBlob(const vec3& center, float radius) : center(center), radius(radius) {}

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] virtual std::pair<vec3, vec3> compute_AABB() const;

    [[nodiscard]] virtual float get_radius() const { return radius; }

    vec3 center;
    float radius;
};

struct CapsuleBlob : Blob {
    CapsuleBlob(const vec3& A, const vec3& B, float radius) : A(A), B(B), radius(radius) {}

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] virtual std::pair<vec3, vec3> compute_AABB() const;

    [[nodiscard]] virtual float get_radius() const { return radius; }

    vec3 A;
    vec3 B;
    float radius;
};
