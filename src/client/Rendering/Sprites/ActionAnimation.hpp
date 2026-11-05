#pragma once

#include <vector>
#include "ActionKey.hpp"
#include "FrameAnchor.hpp"
#include "Playback.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief How an enemy looks in one phase of one attack, as a table
 * `[appearance.attack_<n>.<phase>]` of its sheet describes it: the frames in
 * one row of the sheet, how long each shows, what happens after the last one,
 * and how far the frames hang from the center of the body.
 *
 * An Animation views `frames`, so an action outlives every entity that plays
 * it, as EntityAppearance does.
 */
struct ActionAnimation {
  /** @brief The attack and the phase this look belongs to. */
  game::ActionKey key;
  std::vector<game::SpriteFrame> frames;
  engine::Duration frameDuration{};
  Playback playback{Playback::Once};
  /** @brief Whether the idle animation takes over once this one has ended;
   * otherwise it keeps its last frame, whatever `playback` says. */
  bool returnsToIdle{false};
  FrameAnchor anchor{};
};

}  // namespace rtype::client
