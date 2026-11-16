#pragma once

#include <functional>
#include "ICasinoChannel.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "IWallet.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/**
 * @brief What the casino is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct CasinoEntranceScreenParts {
  /** @brief Screens the casino lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the clicks of its bars, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The table the player sits at once inside, and leaves with LEAVE;
   * it must outlive the screen. */
  std::reference_wrapper<ICasinoChannel> casino;
  /** @brief The wallet of the account signed in, read at every repaint for
   * the balances; it must outlive the screen. */
  std::reference_wrapper<const IWallet> wallet;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Whether the butler moves and the doors open slowly. */
  MotionPreference motion{};
};

}  // namespace rtype::client
