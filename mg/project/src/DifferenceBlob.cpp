/***************************************************************************************************
 * @file  DifferenceBlob.cpp
 * @brief Implementation of the DifferenceBlob class
 **************************************************************************************************/

#include "DifferenceBlob.hpp"

void DifferenceBlob::traverse(std::size_t& count, std::vector<AABB>& aabbs) {
    ++count;

    left->traverse(count, aabbs); // TODO: do better.
};

[[nodiscard]] float DifferenceBlob::potential(const vec3& point) const {
    return std::min(left->potential(point), 2.0f * THRESHOLD - right->potential(point));
}

[[nodiscard]] AABB DifferenceBlob::compute_AABB() const {
    return left->compute_AABB(); // TODO: jsp
}
