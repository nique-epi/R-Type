#include "LastProblem.hpp"
#include <string>
#include "ServerConstants.hpp"

namespace rtype::server {

LastProblem::LastProblem() : logger_(std::string{SERVER_LOGGER_NAME}) {}

void LastProblem::tell(const std::string& problem) {
  if (problem == last_) {
    return;
  }
  last_ = problem;
  logger_.warn(problem);
}

void LastProblem::forget() { last_.clear(); }

}  // namespace rtype::server
