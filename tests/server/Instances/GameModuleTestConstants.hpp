#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

constexpr std::uint32_t MODULE_TEST_SEED = 7;
constexpr std::uint32_t DESTROYED_NETWORK_ID = 9;
constexpr std::uint32_t ENEMY_POINTS = 100;
constexpr std::string_view MODULE_PLAYER_NICKNAME = "Nolan";
constexpr std::uint64_t MODULE_PLAYER_TOKEN = 0x0123456789abcdef;

}  // namespace rtype::server::testing
