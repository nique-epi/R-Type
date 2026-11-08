#pragma once

#include <string>
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief The last problem of the link with the master written in the journal:
 * a master that refuses or does not answer is written once, not at each try,
 * until another problem comes or the link works again.
 */
class LastProblem {
 public:
  LastProblem();

  /** @brief Writes @p problem as a warning unless it was the last one. */
  void tell(const std::string& problem);

  /** @brief Forgets the last problem, so the next one is written even when it
   * is the same: the link worked in between. */
  void forget();

 private:
  logging::Logger logger_;
  std::string last_;
};

}  // namespace rtype::server
