#pragma once

#include <string>

namespace rtype::master {

/** @brief What a route of the API answers: a status and a JSON body. */
struct ApiResponse {
  /** @brief The HTTP status, OK_STATUS when the route succeeded. */
  int status{};
  /** @brief The JSON text of the answer, sent as JSON_CONTENT_TYPE. */
  std::string body;
};

}  // namespace rtype::master
