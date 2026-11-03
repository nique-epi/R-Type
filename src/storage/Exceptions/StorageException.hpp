#pragma once

#include <stdexcept>
#include <string>

namespace rtype::storage {

/** @brief Root of every error raised by the storage library. */
class StorageException : public std::runtime_error {
 public:
  explicit StorageException(const std::string& message);
};

/** @brief SQLite refused to open a database file or to apply a migration. */
class UnopenableDatabaseException : public StorageException {
 public:
  /**
   * @param file The path of the file, as the errors show it.
   * @param reason What SQLite answered.
   */
  UnopenableDatabaseException(const std::string& file,
                              const std::string& reason);
};

/** @brief A database file stays out of WAL, as on a network file system. */
class JournalModeRefusedException : public StorageException {
 public:
  /**
   * @param file The path of the file, as the errors show it.
   * @param mode The journal mode SQLite kept.
   */
  JournalModeRefusedException(const std::string& file, const std::string& mode);
};

/** @brief A database file was last written by a newer version of the program,
 * whose tables this one does not know. */
class NewerDatabaseException : public StorageException {
 public:
  /**
   * @param file The path of the file, as the errors show it.
   * @param foundVersion The number of the last migration it holds.
   */
  NewerDatabaseException(const std::string& file, int foundVersion);
};

}  // namespace rtype::storage
