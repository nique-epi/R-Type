#pragma once

#include <string>
#include <vector>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief How the entities of one type look, as the `[appearance]` table of
 * its sheet describes it: the sprite sheet they are drawn from, the frames of
 * their idle animation, side by side in one row of that sheet, and how long
 * each frame shows.
 *
 * An Animation views idleFrames, so an appearance outlives every entity that
 * plays it: EntityAppearances holds them for as long as the client runs.
 */
struct EntityAppearance {
  /** @brief The asset id of the sprite sheet, as AssetLibrary names it. */
  std::string sheet;
  /** @brief The frames of the idle animation, in the order they show. */
  std::vector<game::SpriteFrame> idleFrames;
  /** @brief How long each idle frame shows. */
  engine::Duration idleFrameDuration{};
};

}  // namespace rtype::client
