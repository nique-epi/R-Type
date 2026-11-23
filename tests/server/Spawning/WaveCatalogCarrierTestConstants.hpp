#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace rtype::server::testing {

/** @brief The keys that bring a crystal carrier into the waves of a
 * difficulty, the shipped carrier as a value, and a Drifter, an enemy that
 * drops no crystal. */
constexpr std::string_view CARRIER_KEY_IN_TESTS = "carrier";
constexpr std::string_view CARRIER_EVERY_KEY_IN_TESTS = "carrier_every";
constexpr std::string_view CRYSTAL_CARRIER_AS_A_VALUE = "\"crystal_carrier\"";
constexpr std::string_view CRYSTAL_CARRIER_NAME_IN_TESTS = "crystal_carrier";
constexpr std::string_view DRIFTER_AS_A_CARRIER = "\"drifter\"";
constexpr std::string_view UNKNOWN_DRIFTER_CARRIER_TEXT =
    "unknown crystal carrier \"drifter\"";

/** @brief What a file without carrier_every reads as, every other wave, and
 * a value of every third wave. */
constexpr std::size_t EVERY_OTHER_WAVE_COUNT = 2;
constexpr std::string_view EVERY_THIRD_WAVE_FOR_A_CARRIER = "3";
constexpr std::size_t EVERY_THIRD_WAVE_FOR_A_CARRIER_COUNT = 3;

/** @brief The difficulties of the shipped waves file. */
constexpr std::array<std::string_view, 3> SHIPPED_DIFFICULTIES{"easy", "normal",
                                                               "hard"};

}  // namespace rtype::server::testing
