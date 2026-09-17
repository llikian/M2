/***************************************************************************************************
 * @file  UnionBlob.cpp
 * @brief Implementation of the UnionBlob class
 **************************************************************************************************/

#include "UnionBlob.hpp"

void UnionBlob::traverse(std::size_t& count, std::vector<AABB>& aabbs) {
    ++count;

    left->traverse(count, aabbs);
    right->traverse(count, aabbs);
};

[[nodiscard]] float UnionBlob::potential(const vec3& point) const {
    return std::max(left->potential(point), right->potential(point));
    // TODO: check which formula should be used
}

[[nodiscard]] AABB UnionBlob::compute_AABB() const {
    return left->compute_AABB(); // TODO: jsp
}
