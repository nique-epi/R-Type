#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The names of the sheets the tests draw, and names they do not. */
constexpr std::string_view DRAWN_ENEMY_NAME = "drifter";
constexpr std::string_view DRAWN_SHIP_NAME = "ship";
constexpr std::string_view OTHER_DRAWN_ENEMY_NAME = "weaver";
constexpr std::string_view UNDRAWN_ENEMY_NAME = "voltjelly";
constexpr std::string_view OTHER_UNDRAWN_ENEMY_NAME = "reaper";
constexpr std::string_view THIRD_UNDRAWN_ENEMY_NAME = "bydomancer";

/** @brief The byte the fingerprint of the client's own content of the tests
 * is made of, and the byte of another content. */
constexpr std::byte LOCAL_FINGERPRINT_BYTE{0x11};
constexpr std::byte OTHER_FINGERPRINT_BYTE{0x22};

/** @brief The folder below the temporary folder of the system the tests that
 * never keep a pack name as the cache: nothing is ever written there. */
constexpr std::string_view UNUSED_CACHE_FOLDER = "r-type-tests-unused-cache";

/** @brief The numbers of the first, the second and the third type of a
 * table. */
constexpr std::uint8_t FIRST_TYPE_NUMBER = 1;
constexpr std::uint8_t SECOND_TYPE_NUMBER = 2;
constexpr std::uint8_t THIRD_TYPE_NUMBER = 3;

}  // namespace rtype::client::testing
