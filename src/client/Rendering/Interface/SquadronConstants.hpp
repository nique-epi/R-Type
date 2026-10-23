#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <string_view>
#include "InterfaceColorConstants.hpp"
#include "SquadronShip.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

/** @brief Size of the squadron box, in pixels of the interface picture. */
constexpr sf::Vector2i SQUADRON_BOX_SIZE(168, 144);

/**
 * @brief Arrival of the squadron, in seconds since it was built: a line grows
 * from the middle of the box until SQUADRON_LINE_END, the box opens until
 * SQUADRON_OPEN_END, SQUADRON is written one letter every
 * SQUADRON_LETTER_TIME from SQUADRON_TITLE_START, the formation starts to
 * drift at SQUADRON_DRIFT_START and drifts fully SQUADRON_DRIFT_RAMP later,
 * and the ships fire from SQUADRON_FIRE_START.
 */
constexpr float SQUADRON_LINE_END = 0.18F;
constexpr float SQUADRON_OPEN_END = 0.45F;
constexpr float SQUADRON_TITLE_START = 0.45F;
constexpr float SQUADRON_LETTER_TIME = 0.05F;
constexpr float SQUADRON_DRIFT_START = 1.9F;
constexpr float SQUADRON_DRIFT_RAMP = 1.0F;
constexpr float SQUADRON_FIRE_START = 2.4F;

/** @brief The opening moves by steps of SQUADRON_OPENING_STEP pixels, and
 * each corner is two strokes SQUADRON_CORNER_LENGTH pixels long. */
constexpr int SQUADRON_OPENING_STEP = 4;
constexpr int SQUADRON_CORNER_LENGTH = 8;

/** @brief Title written in the top-left corner of the box, outlined. */
constexpr std::string_view SQUADRON_TITLE = "SQUADRON";
constexpr sf::Vector2i SQUADRON_TITLE_POSITION(8, 8);
constexpr TextStyle SQUADRON_TITLE_STYLE{
    .font = TextFont::Regular, .color = HUD_CYAN, .outlined = true};

/** @brief The four ships, one per player color, in the order they enter. */
constexpr std::array SQUADRON_SHIPS{
    SquadronShip{.color = 0,
                 .place = {108.0F, 72.0F},
                 .arrival = 0.6F,
                 .bobPhase = 0.0F,
                 .entryDrop = 0.0F},
    SquadronShip{.color = 1,
                 .place = {68.0F, 48.0F},
                 .arrival = 0.78F,
                 .bobPhase = 1.3F,
                 .entryDrop = -8.0F},
    SquadronShip{.color = 2,
                 .place = {68.0F, 96.0F},
                 .arrival = 0.96F,
                 .bobPhase = 2.6F,
                 .entryDrop = 8.0F},
    SquadronShip{.color = 3,
                 .place = {28.0F, 72.0F},
                 .arrival = 1.14F,
                 .bobPhase = 3.9F,
                 .entryDrop = 0.0F},
};

/** @brief A ship enters from SQUADRON_ENTRY_X, in pixels from the left side
 * of the box, and brakes to its place in SQUADRON_FLIGHT_TIME seconds. */
constexpr float SQUADRON_ENTRY_X = -40.0F;
constexpr float SQUADRON_FLIGHT_TIME = 0.65F;

/**
 * @brief Figure eight of the formation: SQUADRON_DRIFT_X pixels each side,
 * SQUADRON_DRIFT_Y up and down, SQUADRON_DRIFT_VERTICAL_LOOPS times per loop,
 * one loop every SQUADRON_DRIFT_PERIOD seconds; each ship also bobs
 * SQUADRON_BOB pixels every SQUADRON_BOB_PERIOD seconds.
 */
constexpr float SQUADRON_DRIFT_X = 14.0F;
constexpr float SQUADRON_DRIFT_Y = 10.0F;
constexpr float SQUADRON_DRIFT_VERTICAL_LOOPS = 2.0F;
constexpr float SQUADRON_DRIFT_PERIOD = 6.4F;
constexpr float SQUADRON_BOB = 2.0F;
constexpr float SQUADRON_BOB_PERIOD = 1.6F;

/**
 * @brief Shots: a random ship in place fires every SQUADRON_SHORTEST_FIRE_GAP
 * to SQUADRON_LONGEST_FIRE_GAP seconds; its missile leaves the muzzle, at
 * SQUADRON_MUZZLE from the top-left corner of the ship, and flies right at
 * SQUADRON_MISSILE_SPEED pixels per second until it leaves the box.
 */
constexpr float SQUADRON_SHORTEST_FIRE_GAP = 1.6F;
constexpr float SQUADRON_LONGEST_FIRE_GAP = 3.0F;
constexpr sf::Vector2f SQUADRON_MUZZLE(30.0F, 5.0F);
constexpr float SQUADRON_MISSILE_SPEED = 200.0F;

}  // namespace rtype::client
