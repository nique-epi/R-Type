#include "ProblemDetails.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include "HttpConstants.hpp"

namespace rtype::master {

std::string problemDetailsText(int status, std::string_view title) {
  nlohmann::json problem;
  problem[PROBLEM_STATUS_MEMBER] = status;
  problem[PROBLEM_TITLE_MEMBER] = title;
  return problem.dump();
}

}  // namespace rtype::master
