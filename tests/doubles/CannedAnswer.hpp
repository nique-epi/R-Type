#pragma once

#include <string>

/** @brief What FakeAccountMaster answers a route with: a status and a body,
 * JSON when there is one. */
struct CannedAnswer {
  int status{};
  std::string body;
};
