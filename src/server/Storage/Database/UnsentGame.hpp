#pragma once

#include <string>
#include "GameResult.hpp"

namespace rtype::server {

/** @brief A vanilla game the master has not accepted yet: the id the file
 * drew for it, which names it on the master too, and its result. */
struct UnsentGame {
  std::string gameId;
  GameResult result;
};

}  // namespace rtype::server
