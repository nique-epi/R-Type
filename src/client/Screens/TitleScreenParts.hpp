#pragma once

#include <cstdint>
#include <functional>
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "SquadronStart.hpp"

namespace rtype::client {

/**
 * @brief What a title screen is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct TitleScreenParts {
  /** @brief Screens the title lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the menu, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief MotionPreference::Reduced shows the squadron at rest. */
  MotionPreference motion{};
  /** @brief Seed of the shots of the squadron. */
  std::uint32_t seed{};
  /** @brief SquadronStart::InPlace opens the title on its squadron in place,
   * as the intro leaves it. */
  SquadronStart squadronStart{};
};

}  // namespace rtype::client
