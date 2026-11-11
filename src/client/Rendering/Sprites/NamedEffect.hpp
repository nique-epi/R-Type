#pragma once

#include <string>
#include <vector>
#include "FrameAnchor.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief An effect the server may ask the client to play once on a body, as a
 * table `[appearance.effects.<name>]` of its sheet describes it: its frames in
 * one row of the sprite sheet of the type, the same frames without flash in
 * the row right under them, how long each shows, and how far they hang from
 * the center of the body.
 */
struct NamedEffect {
  std::string name;
  std::vector<game::SpriteFrame> frames;
  /** @brief The same effect without flash, for REDUCE FLASHES: the row right
   * under frames, frame for frame. */
  std::vector<game::SpriteFrame> framesWithoutFlash;
  engine::Duration frameDuration{};
  FrameAnchor anchor{};
};

}  // namespace rtype::client
