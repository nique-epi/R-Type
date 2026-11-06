#pragma once

#include <cstdint>
#include <string_view>
#include "ConnectionPhase.hpp"
#include "ConnectionRequest.hpp"
#include "ConnectionStatus.hpp"
#include "RecordingServerConnection.hpp"

namespace rtype::client::testing {

/** @brief What the launch options gave: the fields start with them, the
 * port as read and as typed. */
constexpr std::string_view LAUNCH_ADDRESS = "192.168.1.20";
constexpr std::uint16_t LAUNCH_PORT = 4243;
constexpr std::u32string_view LAUNCH_PORT_TEXT = U"4243";
constexpr std::string_view LAUNCH_NICKNAME = "ACE";

/** @brief The game the launch options asked for: game 2 of the server. */
constexpr network::GameChoice LAUNCH_GAME{
    .kind = network::GameChoiceKind::ExistingGame, .number = 2};

/** @brief A port the player types, as typed and as read. */
constexpr std::u32string_view TYPED_PORT_TEXT = U"5000";
constexpr std::uint16_t TYPED_PORT = 5000;

/** @brief A nickname the player types, as typed and as read, and an emptied
 * nickname field, which the screen refuses. */
constexpr std::u32string_view TYPED_NICKNAME_TEXT = U"NOVA";
constexpr std::string_view TYPED_NICKNAME = "NOVA";
constexpr std::u32string_view NO_NICKNAME_TEXT{};

/** @brief A port one below the lowest port. */
constexpr std::u32string_view PORT_ZERO_TEXT = U"0";

/** @brief The last number of the launch address, typed again. */
constexpr std::u32string_view LAST_ADDRESS_NUMBER_TEXT = U"20";

constexpr std::uint8_t THIRD_PLAYER = 3;
constexpr std::uint8_t FIRST_PLAYER = 1;

/** @brief What the connection says once the server accepted the player
 * as the first one. */
constexpr ConnectionStatus ACCEPTED_AS_FIRST_PLAYER{
    .phase = ConnectionPhase::Accepted, .playerNumber = FIRST_PLAYER};

/** @brief The cancel the screen sends when it is built, before any request of
 * a test. */
const ServerConnectionCall CANCEL{
    .request = ServerConnectionCall::Request::Cancel,
    .server = {},
    .nickname = {}};

/** @brief Names of sheets a server plays and this client lacks: two short
 * ones, the longest name of a shipped sheet, which the status line holds whole
 * after MISSING_SHEET_STATUS, and one of the 24 bytes the protocol allows at
 * most, which it does not. */
constexpr std::string_view SHORT_MISSING_SHEET = "voltjelly";
constexpr std::string_view SHORTER_MISSING_SHEET = "reaper";
constexpr std::string_view LONGEST_SHIPPED_MISSING_SHEET = "player_missile";
constexpr std::string_view LONGEST_MISSING_SHEET = "abcdefghijklmnopqrstuvwx";

/** @brief How many other sheets are missing: two, and the 31 a table of the
 * 32 types the protocol allows can name besides the first. */
constexpr std::uint8_t TWO_OTHER_MISSING_SHEETS = 2;
constexpr std::uint8_t MOST_OTHER_MISSING_SHEETS = 31;

}  // namespace rtype::client::testing
