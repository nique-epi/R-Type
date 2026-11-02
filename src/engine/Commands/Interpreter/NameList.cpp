#include "NameList.hpp"
#include <span>
#include <string>

namespace rtype::engine {

namespace {

constexpr char NAME_SEPARATOR = ',';

}  // namespace

std::string joinedNames(std::span<const std::string> names) {
  std::string joined;
  for (const std::string& name : names) {
    if (!joined.empty()) {
      joined += NAME_SEPARATOR;
      joined += ' ';
    }
    joined += name;
  }
  return joined;
}

}  // namespace rtype::engine
