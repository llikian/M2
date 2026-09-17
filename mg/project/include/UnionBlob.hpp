/***************************************************************************************************
 * @file  UnionBlob.hpp
 * @brief Declaration of the UnionBlob class
 **************************************************************************************************/

#pragma once

#include "Blob.hpp"

struct UnionBlob : Blob {
    UnionBlob() : left(nullptr), right(nullptr) {}

    UnionBlob(Blob* left, Blob* right) : left(left), right(right) {}

    void traverse(std::size_t& count, std::vector<AABB>& aabbs) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] AABB compute_AABB() const override;

    Blob* left;
    Blob* right;
};
