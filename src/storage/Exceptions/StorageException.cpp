#include "StorageException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::storage {

StorageException::StorageException(const std::string& message)
    : std::runtime_error(message) {}

UnopenableDatabaseException::UnopenableDatabaseException(
    const std::string& file, const std::string& reason)
    : StorageException("cannot open the database " + file + ": " + reason) {}

JournalModeRefusedException::JournalModeRefusedException(
    const std::string& file, const std::string& mode)
    : StorageException("the database " + file +
                       " cannot use a write-ahead log, it stays in mode " +
                       mode + ": is it on a network file system?") {}

NewerDatabaseException::NewerDatabaseException(const std::string& file,
                                               int foundVersion)
    : StorageException("the database " + file + " holds version " +
                       std::to_string(foundVersion) +
                       ", written by a newer version of this program") {}

}  // namespace rtype::storage
