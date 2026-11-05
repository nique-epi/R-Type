#include "ErrorText.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "DataFilePlace.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::engine {

std::string placeText(const DataFilePlace& place) {
  return place.file + ':' + std::to_string(place.line) + ':' +
         std::to_string(place.column);
}

std::string listText(std::span<const std::string_view> names) {
  if (names.empty()) {
    return "none";
  }
  std::string text{names.front()};
  for (const std::string_view name : names.subspan(1)) {
    text += ", ";
    text += name;
  }
  return text;
}

std::string listText(const std::vector<std::string>& names) {
  const std::vector<std::string_view> views{names.begin(), names.end()};
  return listText(views);
}

std::string rangeText(WholeNumberRange range) {
  return std::to_string(range.lowest) + " to " + std::to_string(range.highest);
}

}  // namespace rtype::engine
