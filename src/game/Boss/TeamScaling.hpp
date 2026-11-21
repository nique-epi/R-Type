#pragma once

#include <cstddef>

namespace rtype::game {

/** @brief What a share is raised by for a team: the percent each player past
 * the first adds, and the players of the team. */
struct TeamScaling {
  int percentPerExtraPlayer{};
  std::size_t players{};
};

}  // namespace rtype::game
