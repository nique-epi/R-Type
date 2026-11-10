#pragma once

#include <vector>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief An idle loop the server may ask a body to take instead of its own,
 * as a table `[appearance.looks.<name>]` of its sheet describes it: its frames
 * in one row of the sprite sheet of the type, and how long each shows. The
 * server and the client reach a look by its number, its place in the order of
 * the names, so the look keeps no name.
 */
struct NamedLook {
  std::vector<game::SpriteFrame> frames;
  engine::Duration frameDuration{};
};

}  // namespace rtype::client
