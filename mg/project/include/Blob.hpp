/***************************************************************************************************
 * @file  Blob.hpp
 * @brief Declaration of the Blob class
 **************************************************************************************************/

#pragma once

#include <vector>
#include "maths/geometry.hpp"
#include "maths/vec3.hpp"
#include "AABB.hpp"

#define THRESHOLD    0.5f
#define WYVILL_COUNT 2
#define GRID_SIZE    0.1f

float attenuation_wyvill(float distance_sqr, int n);
float aabb_radius(float radius);

struct Blob {
    Blob() : negative(false) {}

    virtual ~Blob() = default;

    virtual void traverse(std::size_t& count, std::vector<AABB>& aabbs) = 0;

    [[nodiscard]] virtual float potential(const vec3& point) const = 0;

    [[nodiscard]] virtual AABB compute_AABB() const = 0;

    bool negative;
};

struct SphereBlob : Blob {
    SphereBlob(const vec3& center, float radius) : center(center), radius(radius), radius_sqr(radius * radius) {}

    virtual void traverse(std::size_t& count, std::vector<AABB>& aabbs) override {
        count++;
        aabbs.push_back(compute_AABB());
    }

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] AABB compute_AABB() const override;

    vec3 center;

private:
    float radius;
    float radius_sqr;
};

struct CapsuleBlob : Blob {
    CapsuleBlob(const vec3& A, const vec3& B, float radius) : A(A), B(B), radius(radius), radius_sqr(radius * radius) {}

    virtual void traverse(std::size_t& count, std::vector<AABB>& aabbs) override {
        count++;
        aabbs.push_back(compute_AABB());
    }

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] AABB compute_AABB() const override;

    vec3 A;
    vec3 B;

private:
    float radius;
    float radius_sqr;
};
