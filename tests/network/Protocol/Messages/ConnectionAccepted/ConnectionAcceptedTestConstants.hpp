#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief Names of types at the bounds the protocol page sets: 1 and 24
 * bytes are allowed, 0 and 25 are not. */
constexpr std::string_view SHORTEST_TYPE_NAME = "a";
constexpr std::string_view LONGEST_TYPE_NAME = "abcdefghijklmnopqrstuvwx";
constexpr std::string_view TOO_LONG_TYPE_NAME = "abcdefghijklmnopqrstuvwxy";
constexpr std::string_view EMPTY_TYPE_NAME;

/** @brief Names holding one character the protocol page leaves out of a type
 * name: an uppercase letter, a dash, a space. */
constexpr std::string_view NAME_WITH_AN_UPPERCASE_LETTER = "Drifter";
constexpr std::string_view NAME_WITH_A_DASH = "player-missile";
constexpr std::string_view NAME_WITH_A_SPACE = "enemy missile";

/** @brief A name with every kind of character a type name may hold. */
constexpr std::string_view NAME_OF_EVERY_KIND = "boss_2";

/** @brief The side bytes of the protocol page, and the bytes next to them. */
constexpr std::uint8_t SIDE_BELOW_THE_FIRST = 0;
constexpr std::uint8_t PLAYER_SIDE_BYTE = 1;
constexpr std::uint8_t ENEMY_SIDE_BYTE = 2;
constexpr std::uint8_t ENEMY_MISSILE_SIDE_BYTE = 4;
constexpr std::uint8_t SIDE_ABOVE_THE_LAST = 5;

/** @brief The counts of types around the bound of the protocol page: 1 to 32
 * types are allowed. */
constexpr std::size_t NO_TYPE = 0;
constexpr std::size_t ONE_TYPE = 1;
constexpr std::size_t MOST_TYPES_ON_THE_PROTOCOL_PAGE = 32;
constexpr std::size_t ONE_TYPE_TOO_MANY = 33;

/** @brief The player number of the example acceptances: the first player. */
constexpr std::uint8_t FIRST_PLAYER_BYTE = 1;

/** @brief The largest datagram of a ConnectionAccepted, added up by hand from
 * the protocol page: a header of 22 bytes, a message prefix of 3, then
 * 1 + 1 + 32 x (1 + 1 + 24 + 2 + 2), then the gun, 1 + 1 + 2 + 2 + 4 + 4. */
constexpr std::size_t LARGEST_ACCEPTANCE_ON_THE_PROTOCOL_PAGE = 1001;

/** @brief The bytes a box of a type takes, and the gun, on the protocol page.
 */
constexpr std::size_t NO_BOX_BYTE_COUNT = 4;
constexpr std::size_t NO_GUN_BYTE_COUNT = 14;

/** @brief The numbers of the ship and its missile in acceptanceWithGun(), and
 * a number past its table of two types. */
constexpr std::uint8_t SHIP_TYPE_NUMBER = 1;
constexpr std::uint8_t MISSILE_TYPE_NUMBER = 2;
constexpr std::uint8_t TYPE_NUMBER_PAST_THE_TABLE = 3;

/** @brief The gun of the shipped ship: 20 ticks between shots (333 ms), ready
 * at once, its missile flying right at 960 units per second. */
constexpr std::uint16_t SHIPPED_INTERVAL_TICKS = 20;
constexpr float SHIPPED_MISSILE_SPEED = 960.0F;

}  // namespace rtype::network::testing
