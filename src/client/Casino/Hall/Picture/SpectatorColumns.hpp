#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client {

/** @brief Where a spectator of the crowd of the hall stands: the columns of
 * the picture from `from` up to, but not including, `to`, and its place in
 * the crowd from the left. */
struct SpectatorColumns {
  std::size_t from{};
  std::size_t to{};
  std::uint32_t place{};
};

}  // namespace rtype::client
