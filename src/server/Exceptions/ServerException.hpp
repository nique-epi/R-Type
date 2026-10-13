#pragma once

#include <stdexcept>
#include <string>

namespace rtype::server {

/**
 * @brief Root of every error raised by the server.
 */
class ServerException : public std::runtime_error {
 public:
  explicit ServerException(const std::string& message);
};

/**
 * @brief An argument given at launch that is no option of the server.
 */
class UnknownLaunchArgumentException : public ServerException {
 public:
  explicit UnknownLaunchArgumentException(const std::string& argument);
};

/**
 * @brief A launch option that needs a value was the last argument.
 */
class MissingLaunchOptionValueException : public ServerException {
 public:
  explicit MissingLaunchOptionValueException(const std::string& option);
};

}  // namespace rtype::server
