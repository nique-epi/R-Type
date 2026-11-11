#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief The network id of the body the visual tests pilot. */
constexpr std::uint32_t VISUAL_BODY_NETWORK_ID = 21;

/** @brief The ticks a body rests between two effects: a quarter of a second
 * at 60 ticks a second. */
constexpr std::size_t TICKS_OF_AN_EFFECT_REST = 15;

/** @brief What the sheet of the body names: the effect spark, and the looks
 * angry and cracked, numbered 1 and 2 in the order of their names; then
 * names it does not hold. */
constexpr std::string_view SPARK_EFFECT = "spark";
constexpr std::string_view ANGRY_LOOK = "angry";
constexpr std::string_view CRACKED_LOOK = "cracked";
constexpr std::uint8_t CRACKED_LOOK_NUMBER = 2;
constexpr std::string_view UNNAMED_EFFECT = "smoke";
constexpr std::string_view UNNAMED_LOOK = "melted";
constexpr std::string_view IDLE_LOOK{};
constexpr std::uint8_t IDLE_LOOK_NUMBER = 0;

}  // namespace rtype::game::testing
