#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string_view>
#include "LayerFrame.hpp"

/** @brief The planet of the sky: its image, 157 pixels wide, at (288, 43),
 * in a layer that repeats every 647 columns: the 480 of the background, the
 * 157 of the image and 10 more. */
constexpr std::string_view PLANET_IMAGE = "backgrounds/space/planet.png";
constexpr sf::Vector2f PLANET_PLACE(288.0F, 43.0F);
constexpr sf::Vector2i PLANET_PIXEL(288, 43);
constexpr float PLANET_LAYER_REPEAT = 647.0F;
constexpr float NO_REPEAT = 0.0F;
constexpr std::size_t ONE_FRAME_STRIP = 1;
constexpr std::size_t WHOLE_IMAGE = 0;

/** @brief The layer once the planet has just left on the left: 445 columns
 * moved put its left edge on -157, and the next copy on 490, right of the
 * background. One column more brings no copy in yet; 168 columns moved in
 * all bring the next copy on the last column, 479. */
constexpr rtype::client::LayerFrame PLANET_JUST_GONE{.column = -445};
constexpr rtype::client::LayerFrame NEXT_PLANET_ENTERING{.column = -456};
constexpr sf::Vector2i NEXT_PLANET_ON_THE_LAST_COLUMN(479, 43);
constexpr sf::Vector2i PLANET_TEN_COLUMNS_LEFT(278, 43);
constexpr int LEFT_EDGE_OF_THE_PLANET = 288;
