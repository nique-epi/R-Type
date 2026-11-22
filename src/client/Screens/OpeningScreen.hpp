#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @returns The screen the client opens on: HEADPHONES RECOMMENDED. */
[[nodiscard]] ScreenId openingScreen();

/** @returns The screen HEADPHONES RECOMMENDED gives way to: the intro, or the
 * title screen when the system asks for less motion. */
[[nodiscard]] ScreenId screenAfterTheHeadphones();

}  // namespace rtype::client
