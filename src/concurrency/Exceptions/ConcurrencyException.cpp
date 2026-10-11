#include "ConcurrencyException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::concurrency {

ConcurrencyException::ConcurrencyException(const std::string& message)
    : std::runtime_error(message) {}

InvalidQueueCapacityException::InvalidQueueCapacityException()
    : ConcurrencyException("A bounded queue must hold at least one message") {}

}  // namespace rtype::concurrency
