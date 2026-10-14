#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

/**
 * @brief Reads a pixel of the window as the point of the playfield with the
 * same coordinates: the mapping of a window whose view does not scale.
 */
[[nodiscard]] sf::Vector2f samePlayfieldPoint(sf::Vector2i pixel);

/** @brief The window event of the pointer moving to a pixel of the window. */
[[nodiscard]] sf::Event pointerMove(sf::Vector2i position);

/** @brief The window events of the left mouse button pressed, then released,
 * at a pixel of the window. */
[[nodiscard]] sf::Event leftPress(sf::Vector2i position);
[[nodiscard]] sf::Event leftRelease(sf::Vector2i position);
