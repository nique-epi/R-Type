#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <string_view>
#include "IScreen.hpp"
#include "TimeConstants.hpp"

/** @brief Presses and releases @p key on @p screen. */
void tapKey(rtype::client::IScreen& screen, sf::Keyboard::Key key);

/** @brief Types @p text on @p screen, one character after the other. */
void typeText(rtype::client::IScreen& screen, std::u32string_view text);

/** @brief Presses Backspace @p characters times on @p screen, erasing as many
 * characters from the field it selects. */
void eraseCharacters(rtype::client::IScreen& screen, std::size_t characters);

/** @brief Presses the left button of the pointer over @p interfacePixel, a
 * pixel of the interface picture, on @p screen. */
void clickInterfacePixel(rtype::client::IScreen& screen,
                         sf::Vector2i interfacePixel);

/** @brief Updates @p screen one frame after the other for @p duration, each
 * ONE_FRAME long at most. */
void advanceFrameByFrame(rtype::client::IScreen& screen,
                         rtype::engine::Duration duration);
