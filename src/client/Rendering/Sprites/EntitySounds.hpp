#pragma once

#include <string>
#include <vector>
#include "ActionSound.hpp"

namespace rtype::client {

/**
 * @brief The sounds a sheet names in `[appearance.sounds]`, each an asset id
 * as AssetLibrary names it. A sound the sheet leaves out is empty, and the
 * client plays the sound of the side of the body instead, or none.
 */
struct EntitySounds {
  /** @brief Played as the body appears, in place of the shot of a missile. */
  std::string appear;
  /** @brief Played as the body is killed, in place of the explosion. */
  std::string death;
  /** @brief Played as the body enters an attack phase, one per phase. */
  std::vector<ActionSound> actions;
};

}  // namespace rtype::client
