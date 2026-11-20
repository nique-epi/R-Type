#pragma once

#include <cstdint>
#include <string>

namespace rtype::client {

/** @brief The biggest win of a round, shown on the banner of the table: who
 * won it, and the chips it gained past its stake. */
struct BigWin {
  std::string nickname;
  std::uint32_t gain{};
};

}  // namespace rtype::client
