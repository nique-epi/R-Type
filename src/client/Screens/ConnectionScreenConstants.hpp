#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief Texts of the connection screen: its heading, then the label and the
 * description of each bar, in their order.
 */
constexpr const char* CONNECTION_HEADING = "DIRECT CONNECT";
constexpr const char* ADDRESS_LABEL = "ADDRESS";
constexpr const char* ADDRESS_DESCRIPTION = "> IPV4 ADDRESS OF THE SERVER";
constexpr const char* PORT_LABEL = "PORT";
constexpr const char* PORT_DESCRIPTION = "> UDP PORT OF THE SERVER";
constexpr const char* NICKNAME_LABEL = "PILOT";
constexpr const char* NICKNAME_DESCRIPTION = "> CHOOSE YOUR CALLSIGN";
constexpr const char* CONNECT_LABEL = "CONNECT";
constexpr const char* CONNECT_DESCRIPTION = "> JOIN THE GAME";

/** @brief Order of each field in the menu of the connection screen. */
constexpr std::size_t ADDRESS_ENTRY = 0;
constexpr std::size_t PORT_ENTRY = 1;
constexpr std::size_t NICKNAME_ENTRY = 2;

/**
 * @brief Largest text of each field, in bytes: an IPv4 address of four numbers
 * of three digits and the three dots between them, the five digits of
 * HIGHEST_PORT, and the longest nickname the protocol accepts. A character
 * takes at least one byte, so each fits in its box when its size is within
 * FIELD_CHARACTER_CAPACITY.
 */
constexpr std::size_t ADDRESS_FIELD_SIZE =
    (network::IPV4_ADDRESS_PART_COUNT *
     (static_cast<std::size_t>(std::numeric_limits<std::uint8_t>::digits10) +
      1)) +
    (network::IPV4_ADDRESS_PART_COUNT - 1);
constexpr std::size_t PORT_FIELD_SIZE =
    static_cast<std::size_t>(std::numeric_limits<std::uint16_t>::digits10) + 1;
constexpr std::size_t NICKNAME_FIELD_SIZE = network::MAXIMUM_NICKNAME_SIZE;

static_assert(ADDRESS_FIELD_SIZE <= FIELD_CHARACTER_CAPACITY,
              "the longest address fits in its box");
static_assert(PORT_FIELD_SIZE <= FIELD_CHARACTER_CAPACITY,
              "the highest port fits in its box");
static_assert(NICKNAME_FIELD_SIZE <= FIELD_CHARACTER_CAPACITY,
              "the longest nickname fits in its box");

/**
 * @brief Status lines of the connection screen: how the attempt stands, or
 * which field to fix before one starts. The number of the player follows
 * ACCEPTED_STATUS.
 */
constexpr const char* CONNECTING_STATUS = "CONNECTING...";
constexpr const char* ACCEPTED_STATUS = "CONNECTED AS PLAYER ";
constexpr const char* GAME_FULL_STATUS = "REFUSED: GAME FULL";
constexpr const char* GAME_IN_PROGRESS_STATUS = "REFUSED: GAME IN PROGRESS";
constexpr const char* INCOMPATIBLE_VERSION_STATUS = "REFUSED: OTHER VERSION";
constexpr const char* UNREACHABLE_STATUS = "SERVER UNREACHABLE";
constexpr const char* INVALID_ADDRESS_STATUS = "INVALID ADDRESS";
constexpr const char* INVALID_PORT_STATUS = "INVALID PORT";
constexpr const char* MISSING_NICKNAME_STATUS = "ENTER A CALLSIGN";

/**
 * @brief How long the connection screen shows that the server accepted the
 * player, before it asks for the game.
 */
constexpr engine::Duration ACCEPTED_STATUS_DURATION = std::chrono::seconds(1);

}  // namespace rtype::client
