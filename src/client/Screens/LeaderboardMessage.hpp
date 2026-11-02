#pragma once

namespace rtype::client {

/** @brief What the panel says in place of rows: a line, then one in small
 * print. */
struct LeaderboardMessage {
  const char* line{};
  const char* detail{};
};

}  // namespace rtype::client
