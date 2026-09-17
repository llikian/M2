/***************************************************************************************************
 * @file  IntersectionBlob.hpp
 * @brief Declaration of the IntersectionBlob class
 **************************************************************************************************/

#pragma once

#include "Blob.hpp"

struct IntersectionBlob : Blob {
    IntersectionBlob() : left(nullptr), right(nullptr) {}

    IntersectionBlob(Blob* left, Blob* right) : left(left), right(right) {}

    void traverse(std::size_t& count, std::vector<AABB>& aabbs) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] AABB compute_AABB() const override;

    Blob* left;
    Blob* right;
};
