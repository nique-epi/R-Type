#include "HandOverRuleFixtures.hpp"
#include <algorithm>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "HandOverRule.hpp"

rtype::engine::HandOverRule handingOver(std::vector<std::string> commands) {
  return [commands = std::move(commands)](std::span<const std::string> words) {
    return std::ranges::find(commands, words.front()) != commands.end();
  };
}
