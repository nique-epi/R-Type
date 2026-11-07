#include "ResourceFixtures.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string> textsOf(std::span<const std::string_view> views) {
  return {views.begin(), views.end()};
}
