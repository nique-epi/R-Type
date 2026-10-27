#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include <optional>
#include <set>
#include <utility>
#include <vector>
#include "PilotStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns A teammate called ACE flying, with these lives. */
[[nodiscard]] rtype::client::PilotStatus flying(
    std::uint8_t number, std::optional<std::uint8_t> lives);

/** @returns What the band painted for these pilots. */
[[nodiscard]] InterfaceCanvasRecord bandPaintingOf(
    std::vector<rtype::client::PilotStatus> pilots);

/** @returns The images of the record drawn from this source rectangle. */
[[nodiscard]] std::vector<RecordedImage> imagesOf(
    const InterfaceCanvasRecord& record, sf::IntRect source);

/** @returns Every pixel the fills of this color light, as (column, row). */
[[nodiscard]] std::set<std::pair<int, int>> pixelsIn(
    const InterfaceCanvasRecord& record, sf::Color color);
