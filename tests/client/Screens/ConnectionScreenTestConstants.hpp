#pragma once

#include <cstdint>
#include <string_view>
#include "ConnectionStatus.hpp"
#include "RecordingServerConnection.hpp"

namespace rtype::client::testing {

/** @brief What the launch options gave: the fields start with them, the
 * port as read and as typed. */
constexpr std::string_view LAUNCH_ADDRESS = "192.168.1.20";
constexpr std::uint16_t LAUNCH_PORT = 4243;
constexpr std::u32string_view LAUNCH_PORT_TEXT = U"4243";
constexpr std::string_view LAUNCH_NICKNAME = "ACE";

/** @brief A port the player types, as typed and as read. */
constexpr std::u32string_view TYPED_PORT_TEXT = U"5000";
constexpr std::uint16_t TYPED_PORT = 5000;

/** @brief A port one below the lowest port. */
constexpr std::u32string_view PORT_ZERO_TEXT = U"0";

/** @brief The last number of the launch address, typed again. */
constexpr std::u32string_view LAST_ADDRESS_NUMBER_TEXT = U"20";

constexpr std::uint8_t THIRD_PLAYER = 3;
constexpr std::uint8_t FIRST_PLAYER = 1;

/** @brief What the connection says once the server accepted the player
 * as the first one. */
constexpr ConnectionStatus ACCEPTED_AS_FIRST_PLAYER{
    .phase = ConnectionStatus::Phase::Accepted, .playerNumber = FIRST_PLAYER};

/** @brief The cancel the screen sends when it is built, before any request of
 * a test. */
const ServerConnectionCall CANCEL{
    .request = ServerConnectionCall::Request::Cancel,
    .server = {},
    .nickname = {}};

}  // namespace rtype::client::testing
