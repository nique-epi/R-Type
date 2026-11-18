#pragma once

#include <string>

/** @brief A request FakeAccountMaster received: its method, its path, its
 * Authorization header, empty when it had none, and its body. */
struct RecordedRequest {
  std::string method;
  std::string path;
  std::string authorization;
  std::string body;
};
