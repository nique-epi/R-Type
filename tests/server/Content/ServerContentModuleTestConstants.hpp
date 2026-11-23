#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief The status of a part of an answer (RFC 9110). */
constexpr int PARTIAL_ANSWER = 206;

/** @brief Where a resumed download starts: the bytes already received. */
constexpr std::size_t BYTES_ALREADY_RECEIVED = 10;

/** @brief The end of a range that runs to the end of the body. */
constexpr std::int64_t NO_RANGE_END = -1;

}  // namespace rtype::server::testing
