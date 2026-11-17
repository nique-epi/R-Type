#pragma once

#include <string_view>

/** @brief A call a signed-in player makes on a device link from the web site:
 * the short code of the link, the token of its session, and the body of its
 * decision, empty for a read. */
struct PlayerLinkCall {
  std::string_view userCode;
  std::string_view token;
  std::string_view body;
};
