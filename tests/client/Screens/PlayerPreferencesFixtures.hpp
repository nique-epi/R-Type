#pragma once

#include "ColorMode.hpp"
#include "PlayerSettings.hpp"

/** @returns The settings of a first launch, with this color mode. */
[[nodiscard]] rtype::client::PlayerSettings settingsFor(
    rtype::client::ColorMode mode);
