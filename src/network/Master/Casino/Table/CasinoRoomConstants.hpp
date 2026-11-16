#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace rtype::network {

/** @brief The number of the first roulette room of the casino of the master,
 * and the most rooms and seats per room a master may run. */
constexpr std::uint8_t FIRST_CASINO_ROOM = 1;
constexpr std::uint8_t MAXIMUM_CASINO_ROOMS = 16;
constexpr std::uint8_t MAXIMUM_CASINO_SEATS = 32;

/** @brief The numbers drawn before the current round that the state of a room
 * lists, newest first. */
constexpr std::size_t CASINO_HISTORY_LENGTH = 10;

/**
 * @brief How long a seat lasts once taken or renewed: a seat not renewed for
 * that long no longer counts, so a player who left without getting up frees
 * its place. A client renews its seat every CASINO_SEAT_RENEWAL_INTERVAL.
 */
constexpr std::chrono::seconds CASINO_SEAT_LIFETIME{30};
constexpr std::chrono::seconds CASINO_SEAT_RENEWAL_INTERVAL{10};

}  // namespace rtype::network
