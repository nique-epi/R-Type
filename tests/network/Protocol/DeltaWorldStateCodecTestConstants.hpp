#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network::testing {

/** @brief The values of the example delta: it is built on the state of the
 * protocol page, two ticks back, and removes the entity 5. */
constexpr std::uint8_t EXAMPLE_BASE_DISTANCE = 2;
constexpr std::uint32_t EXAMPLE_REMOVED_ENTITY_ID = 5;

/** @brief Where the header fields sit in a payload: tick (4 bytes), then
 * baseDistance, partIndex and partCount. */
constexpr std::size_t BASE_DISTANCE_OFFSET = 4;
constexpr std::size_t PART_INDEX_OFFSET = 5;
constexpr std::size_t PART_COUNT_OFFSET = 6;

/** @brief The largest baseDistance, and the one just above it. */
constexpr unsigned int LARGEST_BASE_DISTANCE = 60;
constexpr unsigned int BASE_DISTANCE_BEYOND_THE_LARGEST = 61;

/** @brief The largest partCount, and the one just above it. */
constexpr unsigned int LARGEST_PART_COUNT = 16;
constexpr unsigned int PART_COUNT_BEYOND_THE_LARGEST = 17;

/** @brief A partCount of 0, and a part index equal to the count of 2 parts. */
constexpr unsigned int NO_PART_COUNT = 0;
constexpr unsigned int TWO_PARTS = 2;

/** @brief Changed entities one payload holds next to four players:
 * (1175 - 10 - 4 * 16) / 9, rounded down. */
constexpr std::size_t CHANGED_FITTING_WITH_FOUR_PLAYERS = 122;

/** @brief Changed entities that fill two parts with four players. */
constexpr std::size_t CHANGED_FILLING_TWO_PARTS = 200;

/** @brief What is left of those 200 for the second part: 200 - 122. */
constexpr std::size_t CHANGED_LEFT_FOR_THE_SECOND_PART = 78;

/** @brief Changed entities that need one part more than the largest count:
 * 16 * 122 + 1. */
constexpr std::size_t CHANGED_NEEDING_TOO_MANY_PARTS = 1953;

/** @brief The largest count a list of removed ids holds, and the ids that
 * overflow it by 45. */
constexpr std::size_t LARGEST_LIST_COUNT = 255;
constexpr std::size_t REMOVED_IDS_OVERFLOWING_A_LIST = 300;
constexpr std::size_t REMOVED_IDS_LEFT_FOR_THE_SECOND_PART = 45;

/** @brief Bytes before the removed ids of the example payload's last id: the
 * id sits in the last four bytes. */
constexpr std::size_t REMOVED_ID_BYTES = 4;

}  // namespace rtype::network::testing
