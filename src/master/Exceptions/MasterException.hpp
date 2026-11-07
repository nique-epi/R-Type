#pragma once

#include <stdexcept>
#include <string>

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

}  // namespace rtype::master
