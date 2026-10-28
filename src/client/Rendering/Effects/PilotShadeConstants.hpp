#pragma once

#include "InterfaceColorConstants.hpp"
#include "PilotDebrisRamps.hpp"
#include "PilotDebrisRampsOf.hpp"
#include "PilotShades.hpp"
#include "PlayerPaletteConstants.hpp"

namespace rtype::client {

/** @brief The debris of the ship of each player color, in the shades of the
 * sheets or of a palette for color blindness, in the order of PLAYER_COLORS.
 * The shockwave of the ship of a pilot goes through the same colors but the
 * first. */
constexpr PilotDebrisRamps STANDARD_PILOT_DEBRIS_RAMPS =
    pilotDebrisRampsOf(PLAYER_COLORS);
constexpr PilotDebrisRamps PROTANOPIA_PILOT_DEBRIS_RAMPS =
    pilotDebrisRampsOf(PROTANOPIA_PLAYER_PALETTE);
constexpr PilotDebrisRamps DEUTERANOPIA_PILOT_DEBRIS_RAMPS =
    pilotDebrisRampsOf(DEUTERANOPIA_PLAYER_PALETTE);
constexpr PilotDebrisRamps TRITANOPIA_PILOT_DEBRIS_RAMPS =
    pilotDebrisRampsOf(TRITANOPIA_PLAYER_PALETTE);

/** @brief The shades the effects paint pilots in for each color mode: the
 * palette of that mode and the debris ramps made of it. */
constexpr PilotShades STANDARD_PILOT_SHADES{
    .palette = PLAYER_COLORS, .debris = STANDARD_PILOT_DEBRIS_RAMPS};
constexpr PilotShades PROTANOPIA_PILOT_SHADES{
    .palette = PROTANOPIA_PLAYER_PALETTE,
    .debris = PROTANOPIA_PILOT_DEBRIS_RAMPS};
constexpr PilotShades DEUTERANOPIA_PILOT_SHADES{
    .palette = DEUTERANOPIA_PLAYER_PALETTE,
    .debris = DEUTERANOPIA_PILOT_DEBRIS_RAMPS};
constexpr PilotShades TRITANOPIA_PILOT_SHADES{
    .palette = TRITANOPIA_PLAYER_PALETTE,
    .debris = TRITANOPIA_PILOT_DEBRIS_RAMPS};

}  // namespace rtype::client
