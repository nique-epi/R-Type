#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief The score of a player before a kill and after it. */
struct ScoreChange {
  std::uint32_t before{};
  std::uint32_t after{};
};

}  // namespace rtype::server
