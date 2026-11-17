#pragma once

#include <string>
#include <string_view>
#include "SiteClaim.hpp"

/** @brief A call a test makes to a route of the master. */
struct RouteCall {
  std::string_view method;
  std::string_view path;
  /** @brief The key sent as Bearer, none when it is empty. */
  std::string_view key;
  std::string body;
  /** @brief Whether the call comes as Caddy forwards a client that came over
   * HTTPS: it then carries `X-Forwarded-Proto: https`. */
  bool overHttps{false};
  /** @brief The site key and the visitor it names, each sent when it is not
   * empty. */
  rtype::master::SiteClaim site{};
};
