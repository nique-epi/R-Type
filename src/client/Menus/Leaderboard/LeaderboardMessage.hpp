#pragma once

#include "Translation.hpp"

namespace rtype::client {

/** @brief What the panel says in place of rows: a line, then one in small
 * print. */
struct LeaderboardMessage {
  Translation line;
  Translation detail;
};

}  // namespace rtype::client
