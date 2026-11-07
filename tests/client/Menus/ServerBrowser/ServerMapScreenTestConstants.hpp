#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The servers of the map of the tests: Lyon nearer than Paris, both of
 * this version, and an old station of another version, which gives no ping. */
constexpr std::int64_t LYON_SERVER_ID = 8;
constexpr std::int64_t PARIS_SERVER_ID = 7;
constexpr std::int64_t OLD_STATION_ID = 9;
constexpr std::string_view LYON_SERVER_NAME = "LYON-1";
constexpr std::string_view PARIS_SERVER_NAME = "PARIS-1";
constexpr std::string_view OLD_STATION_NAME = "OLD-STATION";
constexpr std::string_view LYON_SERVER_ADDRESS = "203.0.113.8";
constexpr std::string_view PARIS_SERVER_ADDRESS = "203.0.113.7";
constexpr std::string_view OLD_STATION_ADDRESS = "203.0.113.9";
constexpr std::uint16_t SERVER_GAME_PORT = 4242;
constexpr std::chrono::milliseconds LYON_PING{24};
constexpr std::chrono::milliseconds PARIS_PING{88};
/** @brief Where Paris comes in the list of three servers, sorted by ping. */
constexpr std::size_t PARIS_INDEX = 1;
constexpr std::uint8_t PLAYERS_PER_GAME = 4;

/** @brief The games of Lyon: one waiting, one in game. */
constexpr std::uint16_t WAITING_GAME_NUMBER = 1;
constexpr std::uint16_t PLAYING_GAME_NUMBER = 2;
constexpr std::string_view WAITING_GAME_NAME = "ALPHA WING";
constexpr std::string_view PLAYING_GAME_NAME = "LAST STAND";
constexpr std::uint8_t TWO_PILOTS = 2;

/** @brief A frame, the time after which the map reads the list again on its
 * own (15 s), and a time short of it. */
constexpr std::chrono::milliseconds ONE_FRAME{16};
constexpr std::chrono::seconds REFRESH_INTERVAL{15};
constexpr std::chrono::seconds SHORT_OF_THE_REFRESH{14};

/** @brief The words the map writes when the master is unreachable. */
constexpr std::string_view UNREACHABLE_MASTER = "MASTER UNREACHABLE";

/** @brief The master the map reads, as its url and as the map writes it. */
constexpr std::string_view MAP_MASTER_URL = "http://10.0.0.2:8080";
constexpr std::string_view MAP_MASTER_TEXT = "10.0.0.2:8080";

/** @brief Seven games, one more than the panel shows at once. */
constexpr std::size_t SEVEN_GAMES = 7;
constexpr std::uint16_t SEVENTH_GAME_NUMBER = 7;
constexpr int SIX_ROWS_DOWN = 6;

}  // namespace rtype::client::testing
