#pragma once

#include <string>

namespace rtype::client {

/** @brief The master the server map reads. */
class IMasterAddress {
 public:
  virtual ~IMasterAddress() = default;

  /** @returns Its url, `http(s)://<host>[:<port>]`. */
  [[nodiscard]] virtual std::string url() const = 0;
};

}  // namespace rtype::client
