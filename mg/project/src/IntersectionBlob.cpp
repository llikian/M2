/***************************************************************************************************
 * @file  IntersectionBlob.cpp
 * @brief Implementation of the IntersectionBlob class
 **************************************************************************************************/

#include "IntersectionBlob.hpp"

#include "maths/functions.hpp"

void IntersectionBlob::traverse(std::size_t& count, std::vector<AABB>& aabbs) {
    ++count;

    std::vector<AABB> left_aabbs;
    std::vector<AABB> right_aabbs;
    left->traverse(count, left_aabbs);
    right->traverse(count, right_aabbs);

    for(const auto& A : left_aabbs) {
        for(const auto& B : right_aabbs) { aabbs.emplace_back(max(A.min, B.min), min(A.max, B.max)); }
    }
};

[[nodiscard]] float IntersectionBlob::potential(const vec3& point) const {
    return std::min(left->potential(point), right->potential(point));
}

[[nodiscard]] AABB IntersectionBlob::compute_AABB() const {
    return left->compute_AABB(); // TODO: jsp
}
