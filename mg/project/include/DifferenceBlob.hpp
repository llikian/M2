/***************************************************************************************************
 * @file  DifferenceBlob.hpp
 * @brief Declaration of the DifferenceBlob class
 **************************************************************************************************/

#pragma once

#include "Blob.hpp"

struct DifferenceBlob : Blob {
    DifferenceBlob() : left(nullptr), right(nullptr) {}

    DifferenceBlob(Blob* left, Blob* right) : left(left), right(right) {}

    void traverse(std::size_t& count, std::vector<AABB>& aabbs) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] AABB compute_AABB() const override;

    Blob* left;
    Blob* right;
};
