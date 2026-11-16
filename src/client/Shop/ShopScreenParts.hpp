#pragma once

#include <functional>
#include "IAudioService.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "IWallet.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/**
 * @brief What the shop is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ShopScreenParts {
  /** @brief Screens the shop lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the clicks and the sound of a purchase, which must outlive
   * the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Plays the music of the shop, or of its radio, while the shop
   * shows; it must outlive the screen. */
  std::reference_wrapper<IAudioService> audio;
  /** @brief The kopecks and the looks of the account signed in, which buys
   * through it; it must outlive the screen. */
  std::reference_wrapper<IWallet> wallet;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Whether the shopkeeper may move, or the player asked the system
   * for less motion. */
  MotionPreference motion{};
};

}  // namespace rtype::client
