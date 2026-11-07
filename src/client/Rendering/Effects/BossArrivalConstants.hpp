#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "EffectColorConstants.hpp"
#include "FireColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "RenderingConstants.hpp"
#include "Ring.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Times since the Arriving status: the camera rumbles from
 * BOSS_RUMBLE_START to BOSS_RUMBLE_END, its trauma kept at
 * BOSS_RUMBLE_TRAUMA (about 1 pixel); the boss walks in from
 * BOSS_ENTRY_START for BOSS_ENTRY_TIME, as the server moves it, a jolt of
 * BOSS_JOLT_TRAUMA (about 2 pixels) every BOSS_JOLT_PERIOD; its roar adds
 * BOSS_ROAR_TRAUMA (about 3 pixels). */
constexpr engine::Duration BOSS_RUMBLE_START = std::chrono::milliseconds(2400);
constexpr engine::Duration BOSS_RUMBLE_END = std::chrono::milliseconds(3800);
constexpr float BOSS_RUMBLE_TRAUMA = 0.41F;
constexpr engine::Duration BOSS_ENTRY_START = std::chrono::milliseconds(4400);
constexpr engine::Duration BOSS_ENTRY_TIME = std::chrono::milliseconds(1800);
constexpr engine::Duration BOSS_JOLT_PERIOD = std::chrono::milliseconds(450);
constexpr int BOSS_JOLT_COUNT =
    static_cast<int>(BOSS_ENTRY_TIME / BOSS_JOLT_PERIOD);
constexpr float BOSS_JOLT_TRAUMA = 0.58F;
constexpr float BOSS_ROAR_TRAUMA = 0.71F;

/** @brief The attack whose strike is the roar of the boss: the strike of its
 * arrival (TYRANT_ARRIVAL_ATTACK on the server). */
constexpr std::uint8_t BOSS_ROAR_ATTACK = 0;

/** @brief The mouth of the boss in its BOSS_FRAME_SIDE square frame, and how
 * far it is from the center the frame is drawn on, in logical units. */
constexpr sf::Vector2i BOSS_MOUTH_IN_FRAME(33, 75);
constexpr int BOSS_FRAME_SIDE = 128;
constexpr sf::Vector2f BOSS_MOUTH_OFFSET =
    (sf::Vector2f(BOSS_MOUTH_IN_FRAME) -
     (sf::Vector2f(BOSS_FRAME_SIDE, BOSS_FRAME_SIDE) * HALF)) *
    BACKGROUND_PIXEL_SIZE;

/** @brief The two shockwaves of the roar, over ROAR_RING_LIFE: one of bone,
 * then, ROAR_ORANGE_RING_DELAY later, one of orange opening to
 * ROAR_ORANGE_RING_PERCENT of its radius. */
constexpr sf::Color ROAR_BONE(227, 211, 166);
constexpr std::array ROAR_BONE_RAMP{ROAR_BONE, ASTEROID_LIGHT_COLOR,
                                    ASTEROID_SHADOW_COLOR, SMOKE};
constexpr std::array ROAR_ORANGE_RAMP{WARNING_ORANGE, FIRE_RED, EMBER, SMOKE};
constexpr int ROAR_RING_RADIUS = 64;
constexpr int ROAR_ORANGE_RING_PERCENT = 72;
constexpr int WHOLE_PERCENT = 100;
constexpr engine::Duration ROAR_RING_LIFE = std::chrono::milliseconds(1000);
constexpr engine::Duration ROAR_ORANGE_RING_DELAY =
    std::chrono::milliseconds(150);
constexpr Ring ROAR_BONE_RING{.radius = ROAR_RING_RADIUS,
                              .life = ROAR_RING_LIFE};
constexpr Ring ROAR_ORANGE_RING{
    .radius = (ROAR_RING_RADIUS * ROAR_ORANGE_RING_PERCENT) / WHOLE_PERCENT,
    .life = ROAR_RING_LIFE - ROAR_ORANGE_RING_DELAY};

}  // namespace rtype::client
