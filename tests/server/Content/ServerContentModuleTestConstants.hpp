#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The statuses of a whole answer and of a part of one (RFC 9110). */
constexpr int WHOLE_ANSWER = 200;
constexpr int PARTIAL_ANSWER = 206;

/** @brief Where a resumed download starts: the bytes already received. */
constexpr std::size_t BYTES_ALREADY_RECEIVED = 10;

/** @brief The end of a range that runs to the end of the body. */
constexpr std::int64_t NO_RANGE_END = -1;

/** @brief A sheet the assets of the tests hold. */
constexpr std::string_view DRIFTER_SHEET = "entities/drifter.toml";

}  // namespace rtype::server::testing
