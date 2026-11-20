#pragma once

#include <functional>
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
  /** @brief How far from the left of the bubble its tail points down. */
  int tail{};
  /** @brief Whether the line is written letter by letter, or the player
   * asked for less motion and it shows whole at once. */
  MotionPreference motion{};
  /** @brief Whether a line stays once whole, until another is said, as in
   * the panel of a character who keeps talking to the player; otherwise it
   * goes SPOKEN_LINGER_TIME after. */
  bool stays{};
};

}  // namespace rtype::client
