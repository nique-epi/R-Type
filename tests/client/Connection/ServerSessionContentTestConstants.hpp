#pragma once

#include <chrono>
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

/** @brief How long a test waits for a pack to load, and the frame it runs
 * meanwhile. */
constexpr std::chrono::seconds LOADING_WAIT{20};
constexpr std::chrono::milliseconds LOADING_FRAME{10};

/** @brief The status of a server that refuses the pack. */
constexpr int PACK_NOT_FOUND_STATUS = 404;

/** @brief The port of a supervisor that serves no pack: its HTTP server did
 * not start. */
constexpr std::uint16_t NO_PACK_PORT = 0;

/** @brief The size of a pack a test announces and never downloads. */
constexpr std::uint32_t ANY_PACK_SIZE = 1;

/** @brief The seed of a pack a server sends in place of the one it
 * announced (see packNumbered()). */
constexpr std::size_t ANOTHER_PACK_SEED = 7;

}  // namespace rtype::client::testing
