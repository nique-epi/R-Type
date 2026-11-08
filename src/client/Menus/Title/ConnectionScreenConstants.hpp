#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "InterfaceConstants.hpp"
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
 * ACCEPTED_STATUS. A refusal for another version names both protocol
 * versions: the client's after INCOMPATIBLE_VERSION_STATUS, the server's after
 * SERVER_VERSION_STATUS.
 */
constexpr const char* CONNECTING_STATUS = "CONNECTING...";
constexpr const char* ACCEPTED_STATUS = "CONNECTED AS PLAYER ";
constexpr const char* GAME_FULL_STATUS = "REFUSED: GAME FULL";
constexpr const char* GAME_IN_PROGRESS_STATUS = "REFUSED: GAME IN PROGRESS";
constexpr const char* NICKNAME_TAKEN_STATUS = "REFUSED: CALLSIGN TAKEN";
constexpr const char* GAME_NOT_FOUND_STATUS = "REFUSED: NO SUCH GAME";
constexpr const char* SERVER_FULL_STATUS = "REFUSED: SERVER FULL";
constexpr const char* UNKNOWN_DIFFICULTY_STATUS = "REFUSED: BAD DIFFICULTY";
/** @brief The refusal of lives out of bounds, then the fewest lives a server
 * accepts, LIVES_BOUNDS_SEPARATOR and the most it accepts. */
constexpr const char* LIVES_REFUSED_STATUS = "REFUSED: LIVES ";
constexpr const char* LIVES_BOUNDS_SEPARATOR = " TO ";
constexpr const char* BANNED_STATUS = "REFUSED: BANNED";
constexpr const char* INCOMPATIBLE_VERSION_STATUS = "REFUSED: V";
constexpr const char* SERVER_VERSION_STATUS = ", SERVER V";
constexpr const char* UNREACHABLE_STATUS = "SERVER UNREACHABLE";
constexpr const char* INVALID_ADDRESS_STATUS = "INVALID ADDRESS";
constexpr const char* INVALID_PORT_STATUS = "INVALID PORT";
constexpr const char* MISSING_NICKNAME_STATUS = "ENTER A CALLSIGN";

/**
 * @brief The status lines of the content of a server: LOADING_CONTENT_STATUS
 * then the share of the pack received, in whole percents, while it downloads;
 * then, when it cannot be loaded, why.
 */
constexpr const char* LOADING_CONTENT_STATUS = "LOADING CONTENT ";
constexpr const char* PERCENT_SIGN = "%";
constexpr std::size_t WHOLE_PERCENT = 100;
constexpr const char* PACK_DOWNLOAD_FAILED_STATUS = "PACK DOWNLOAD FAILED";
constexpr const char* PACK_REFUSED_STATUS = "PACK REFUSED";

/**
 * @brief The characters a status line of the form holds: the width of the
 * form less its margin on both sides, 240 - 2 x 16 = 208 pixels, over the 8
 * pixels a character of the regular font takes, so 26. The refusal of a
 * version, REFUSED: V14, SERVER V15, takes 24, and 26 for versions of three
 * digits.
 */
constexpr std::size_t STATUS_LINE_CHARACTERS = static_cast<std::size_t>(
    (FORM_WIDTH - (SIDES_PER_AXIS * FORM_HEADING_OFFSET.x)) /
    REGULAR_FONT_ADVANCE);

static_assert(std::string_view(UNKNOWN_DIFFICULTY_STATUS).size() <=
                  STATUS_LINE_CHARACTERS,
              "the refusal of a difficulty fits its status line");
static_assert(std::string_view(LIVES_REFUSED_STATUS).size() +
                      std::string_view(LIVES_BOUNDS_SEPARATOR).size() +
                      static_cast<std::size_t>(
                          SIDES_PER_AXIS *
                          (std::numeric_limits<std::uint8_t>::digits10 + 1)) <=
                  STATUS_LINE_CHARACTERS,
              "the refusal of lives fits its status line, with two bounds of "
              "three digits");

/**
 * @brief How long the connection screen shows that the server accepted the
 * player, before it opens the waiting room.
 */
constexpr engine::Duration ACCEPTED_STATUS_DURATION = std::chrono::seconds(1);

}  // namespace rtype::client
