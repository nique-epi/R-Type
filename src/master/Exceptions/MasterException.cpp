#include "MasterException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::master {

MasterException::MasterException(const std::string& message)
    : std::runtime_error(message) {}

DatabaseClosedException::DatabaseClosedException()
    : MasterException("the database of the master is not open") {}

}  // namespace rtype::master
