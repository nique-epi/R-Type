#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @returns The screen the client opens on: the intro, or the title screen
 * when the system asks for less motion. */
[[nodiscard]] ScreenId openingScreen();

}  // namespace rtype::client
