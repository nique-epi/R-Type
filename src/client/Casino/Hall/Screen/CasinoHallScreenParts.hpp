#pragma once

#include <functional>
#include "HallPicture.hpp"
#include "IAudioService.hpp"
#include "ICasinoChannel.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/**
 * @brief What the hall is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct CasinoHallScreenParts {
  /** @brief Screens the hall lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the clicks of its bars, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Plays the music of the hall and its crowd while the screen shows;
   * it must outlive the screen. */
  std::reference_wrapper<IAudioService> audio;
  /** @brief The casino whose rooms the hall lists, which ROULETTE enters
   * and BACK leaves; it must outlive the screen. */
  std::reference_wrapper<ICasinoChannel> casino;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief What the hall read in its picture; empty, the hall stays
   * still. */
  HallPicture picture;
  /** @brief Whether the hall moves and fades before it opens a room. */
  MotionPreference motion{};
};

}  // namespace rtype::client
