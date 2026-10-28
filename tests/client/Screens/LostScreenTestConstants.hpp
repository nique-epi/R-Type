#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include "NoSignalCardTestConstants.hpp"
#include "TimeConstants.hpp"

/** @brief The screen as the design times it: dark 450 ms after the client
 * gave up, then the card, whose call is typed CALL_TYPED later; the middle of
 * the call on the interface picture. */
constexpr rtype::engine::Duration SCREEN_TURNED_OFF =
    std::chrono::milliseconds(450);
constexpr rtype::engine::Duration WHOLE_CALL_TYPED =
    SCREEN_TURNED_OFF + CALL_TYPED;
constexpr sf::Vector2i MIDDLE_OF_THE_CALL(240, 106);
constexpr sf::Vector2i CORNER_OF_THE_PICTURE(2, 2);
