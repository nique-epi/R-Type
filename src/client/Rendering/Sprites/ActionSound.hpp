#pragma once

#include <string>
#include "ActionKey.hpp"

namespace rtype::client {

/**
 * @brief The sound a sheet names for one phase of one attack, in
 * `[appearance.sounds.attack_<n>]`: the client plays it as the body enters
 * that phase.
 */
struct ActionSound {
  /** @brief The attack and the phase the sound starts with. */
  game::ActionKey key;
  /** @brief The asset id of the sound, as AssetLibrary names it. */
  std::string sound;
};

}  // namespace rtype::client
