#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief The numbers of the ordered channel the tests use: the first two. */
constexpr std::uint16_t FIRST_ORDERED = 1;
constexpr std::uint16_t SECOND_ORDERED = 2;

/** @brief What the messages of the tests name. */
constexpr std::uint32_t DESTROYED_ENTITY_ID = 7;
constexpr std::uint8_t HIT_PLAYER_NUMBER = 1;
constexpr std::uint16_t SOME_EVENT_NUMBER = 1;
constexpr std::uint32_t SOME_SENT_AT = 1000;

}  // namespace rtype::client::testing
