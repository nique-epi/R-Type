#pragma once

namespace rtype::game {

/** @brief The rows a barrel roll shows of a frame `height` rows tall: how
 * many, and whether they are upside down, the belly of the ship showing. */
struct RolledRows {
  int height{};
  int count{};
  bool belly{false};
};

}  // namespace rtype::game
