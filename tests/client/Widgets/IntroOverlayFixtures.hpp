#pragma once

#include "InterfaceColorConstants.hpp"
#include "PlayerPalette.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns What paintIntroOverlay() asks a canvas at @p real seconds of the
 * music, with the logo of the title, the pilots in @p palette. */
[[nodiscard]] InterfaceCanvasRecord overlayAt(
    float real,
    const rtype::client::PlayerPalette& palette = rtype::client::PLAYER_COLORS);
