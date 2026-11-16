#pragma once

#include <cstdint>
#include "RouletteBet.hpp"

namespace rtype::master {

/** @brief One bet of a round, with the account that laid it. */
struct PlayerBet {
  std::int64_t userId{};
  network::RouletteBet bet;
};

}  // namespace rtype::master
