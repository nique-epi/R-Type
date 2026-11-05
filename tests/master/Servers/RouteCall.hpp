#pragma once

#include <string>
#include <string_view>

/** @brief A call a test makes to a route of the master. */
struct RouteCall {
  std::string_view method;
  std::string_view path;
  /** @brief The key sent as Bearer, none when it is empty. */
  std::string_view key;
  std::string body;
};
