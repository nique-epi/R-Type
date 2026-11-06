#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The secret of the server of the tests, another one, and tokens. */
constexpr std::uint64_t DESK_SECRET = 0x0102'0304'0506'0708;
constexpr std::uint64_t ANOTHER_SECRET = 0x0807'0605'0403'0201;
constexpr std::uint64_t FIRST_TOKEN = 0x1111'1111'1111'1111;
constexpr std::uint64_t SECOND_TOKEN = 0x2222'2222'2222'2222;

/** @brief Where the probes of the tests come from, and when they arrive. */
constexpr std::string_view MASTER_ADDRESS = "198.51.100.20";
constexpr std::uint16_t MASTER_PORT = 50'000;
constexpr std::chrono::nanoseconds PROBE_ARRIVAL{1'000'000};

}  // namespace rtype::server::testing
