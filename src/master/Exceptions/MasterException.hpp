#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace rtype::master {

/** @brief Root of every error raised by the master. */
class MasterException : public std::runtime_error {
 public:
  explicit MasterException(const std::string& message);
};

/**
 * @brief A request read the database of the master while it is not open:
 * before the storage module started, or after it stopped.
 */
class DatabaseClosedException : public MasterException {
 public:
  DatabaseClosedException();
};

/**
 * @brief The master was launched without a key of the passwords while its
 * module accounts is on: it stops at its start, before its port opens.
 */
class MissingPasswordKeyException : public MasterException {
 public:
  /** @param variable The environment variable the key is read from. */
  explicit MissingPasswordKeyException(std::string_view variable);
};

/**
 * @brief The key of the passwords given at launch is no key: the message names
 * the variable that holds it, never its value.
 */
class InvalidPasswordKeyException : public MasterException {
 public:
  /** @param variable The environment variable the key was read from. */
  explicit InvalidPasswordKeyException(std::string_view variable);
};

}  // namespace rtype::master
