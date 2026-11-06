#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief What the frame loop asked of the master so far, as MasterLink hands
 * it to its thread: each ask has a number one above the previous one of its
 * kind, so the thread serves the newest ask only, and a reading tells which
 * ask it answers. 0 means not asked yet.
 */
struct MasterAsks {
  std::uint64_t serverAsk{};
  std::uint64_t gameAsk{};
  /** @brief The server whose games gameAsk asks for. */
  std::int64_t gameServerId{};
};

}  // namespace rtype::client
