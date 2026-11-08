#pragma once

namespace rtype::client {

/** @brief Words of the line that names the rules of a game after its
 * difficulty: the separator, the lives for one and for several, and the
 * friendly fire on and off. */
constexpr const char* RULES_SEPARATOR = ", ";
constexpr const char* ONE_LIFE_WORD = " LIFE";
constexpr const char* LIVES_WORD = " LIVES";
constexpr const char* FRIENDLY_FIRE_ON_WORDS = "FRIENDLY FIRE ON";
constexpr const char* FRIENDLY_FIRE_OFF_WORDS = "FRIENDLY FIRE OFF";

}  // namespace rtype::client
