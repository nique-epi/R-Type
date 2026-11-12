#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ConnectionPhase.hpp"
#include "ConnectionRequest.hpp"
#include "ConnectionStatus.hpp"
#include "RecordingServerConnection.hpp"
#include "ServerConnectionRequest.hpp"

namespace rtype::client::testing {

/** @brief What the launch options gave: the fields start with them, the
 * port as read and as typed. */
constexpr std::string_view LAUNCH_ADDRESS = "192.168.1.20";
constexpr std::uint16_t LAUNCH_PORT = 4243;
constexpr std::u32string_view LAUNCH_PORT_TEXT = U"4243";

/** @brief The server the server map chose, other than the launch one. */
constexpr std::string_view MAP_ADDRESS = "203.0.113.9";
constexpr std::uint16_t MAP_PORT = 4300;
/** @brief The server QUICK PLAY keeps behind the one it chose. */
constexpr std::string_view NEXT_MAP_ADDRESS = "203.0.113.10";
constexpr std::uint16_t NEXT_MAP_PORT = 4301;
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

/** @brief The labels of the address field and of the nickname field, as the
 * player reads them. */
constexpr std::string_view ADDRESS_FIELD_LABEL = "ADDRESS";
constexpr std::string_view NICKNAME_FIELD_LABEL = "PILOT";

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
    .request = ServerConnectionRequest::Cancel, .server = {}, .nickname = {}};

/** @brief A content pack of 150 000 bytes, nothing of it received, then
 * half of it, then all of it. */
constexpr std::size_t PACK_SIZE = 150'000;
constexpr std::size_t NOTHING_RECEIVED = 0;
constexpr std::size_t HALF_RECEIVED = 75'000;

/** @brief The status lines the two refusals of the rules of a new game must
 * read, as a player sees them: the bounds of the lives written out. */
constexpr std::string_view BAD_DIFFICULTY_REFUSAL = "REFUSED: BAD DIFFICULTY";
constexpr std::string_view LIVES_BOUNDS_REFUSAL = "REFUSED: LIVES 1 TO 9";

}  // namespace rtype::client::testing
