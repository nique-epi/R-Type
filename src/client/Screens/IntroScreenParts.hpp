#pragma once

#include <cstdint>
#include <functional>
#include "IMusicPlayer.hpp"
#include "IScreenNavigator.hpp"
#include "ISky.hpp"
#include "ISoundPlayer.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief What the intro is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct IntroScreenParts {
  /** @brief Screens the intro lives among, which must outlive it: the title
   * replaces it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Settings of the player, read when the intro is built: REDUCE
   * FLASHES plays it calm. */
  std::reference_wrapper<const PlayerSettings> settings;
  /** @brief The sky behind the screens, which must outlive the intro: it
   * speeds up, slows down and shakes with the scene, and is put back when
   * the intro goes. */
  std::reference_wrapper<ISky> sky;
  /** @brief Plays the sounds of the intro; it must outlive the intro. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The music, which the intro starts after its black second and
   * follows; it must outlive the intro. */
  std::reference_wrapper<IMusicPlayer> music;
  /** @brief Seed of the sparks of the bursts. */
  std::uint32_t seed{};
};

}  // namespace rtype::client
