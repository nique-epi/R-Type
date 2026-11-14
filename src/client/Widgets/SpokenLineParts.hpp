#pragma once

#include <functional>
#include <optional>
#include <span>
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"

namespace rtype::client {

/**
 * @brief What a spoken line is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct SpokenLineParts {
  /** @brief Plays the voice; it must outlive the line. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The sounds of the voice, asset ids of SoundConstants.hpp played
   * in turn, one as each word starts; the ids must outlive the line. */
  std::span<const char* const> voice;
  /** @brief How wide the bubble is, its border included. */
  int width{};
  /** @brief How far from the left of the bubble its tail points down; no
   * tail when empty. */
  std::optional<int> tail = std::nullopt;
  /** @brief Whether the line is written letter by letter, or the player
   * asked for less motion and it shows whole at once. */
  MotionPreference motion{};
};

}  // namespace rtype::client
