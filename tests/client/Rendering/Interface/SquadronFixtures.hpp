#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "InterfaceColorConstants.hpp"
#include "PlayerPalette.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "Squadron.hpp"
#include "TimeConstants.hpp"

/** @brief Advances a squadron frame by frame, so no step is capped. */
void play(rtype::client::Squadron& squadron, rtype::engine::Duration total);

/** @returns What a squadron paints in BOX, its ships in @p palette. */
[[nodiscard]] InterfaceCanvasRecord paintingOf(
    const rtype::client::Squadron& squadron,
    const rtype::client::PlayerPalette& palette = rtype::client::PLAYER_COLORS);

/** @returns The top-left corners of the images of a size, in drawing
 * order. */
[[nodiscard]] std::vector<sf::Vector2i> cornersOf(
    const InterfaceCanvasRecord& record, sf::Vector2i size);

/** @returns The places of the four ships, in pixels of the picture. */
[[nodiscard]] std::vector<sf::Vector2i> formationInBox();
