#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client::testing {

/** @brief The sequence the server writes in a second copy of its acceptance,
 * newer than the first (0), so the copy is read. */
constexpr std::uint16_t REPEATED_ACCEPTANCE_SEQUENCE = 1;

/** @brief The copies of a Disconnection the protocol page sends, three, and
 * the ones that follow the first. */
constexpr std::size_t DISCONNECTION_COPIES_SENT = 3;
constexpr std::size_t DISCONNECTION_COPIES_AFTER_THE_FIRST = 2;

}  // namespace rtype::client::testing
