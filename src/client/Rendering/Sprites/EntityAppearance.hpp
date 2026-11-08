#pragma once

#include <string>
#include <vector>
#include "ActionAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief How the entities of one type look, as the `[appearance]` table of
 * its sheet describes it: the sprite sheet they are drawn from, the frames of
 * their idle animation, side by side in one row of that sheet, how long each
 * frame shows, the animation of each phase of each attack they have, and
 * their death when the sheet draws one.
 *
 * An Animation views idleFrames and the frames of an action, so an appearance
 * outlives every entity that plays it: EntityAppearances holds them for as long
 * as the client runs.
 */
struct EntityAppearance {
  /** @brief The asset id of the sprite sheet, as AssetLibrary names it. */
  std::string sheet;
  /** @brief The frames of the idle animation, in the order they show. */
  std::vector<game::SpriteFrame> idleFrames;
  /** @brief How long each idle frame shows. */
  engine::Duration idleFrameDuration{};
  /** @brief The animation of every attack phase the sheet describes, none for
   * an entity that does not attack. */
  std::vector<ActionAnimation> actions;
  /** @brief The frames of the idle animation once the enemy is enraged; none
   * when the sheet has no `[appearance.enraged_idle]`. */
  std::vector<game::SpriteFrame> enragedIdleFrames;
  /** @brief How long each enraged idle frame shows. */
  engine::Duration enragedIdleFrameDuration{};
  /** @brief The frames of the death, played once instead of the Bydo
   * explosion; none when the sheet has no `[appearance.death]`. */
  std::vector<game::SpriteFrame> deathFrames;
  /** @brief The same death without flash, for REDUCE FLASHES: the row right
   * under deathFrames, frame for frame. */
  std::vector<game::SpriteFrame> deathFramesWithoutFlash;
  /** @brief How long each frame of the death shows. */
  engine::Duration deathFrameDuration{};
};

}  // namespace rtype::client
