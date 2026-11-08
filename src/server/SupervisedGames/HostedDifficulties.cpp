#include "HostedDifficulties.hpp"
#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rtype::server {

void HostedDifficulties::keep(std::vector<std::string> names) {
  names_ = std::move(names);
}

bool HostedDifficulties::contains(std::string_view name) const {
  return std::ranges::find(names_, name) != names_.end();
}

const std::vector<std::string>& HostedDifficulties::names() const {
  return names_;
}

}  // namespace rtype::server
