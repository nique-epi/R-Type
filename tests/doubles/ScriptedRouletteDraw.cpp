#include "ScriptedRouletteDraw.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace rtype::server::testing {

ScriptedRouletteDraw::ScriptedRouletteDraw(std::vector<std::uint8_t> numbers)
    : numbers_(std::move(numbers)) {}

std::uint8_t ScriptedRouletteDraw::drawNumber() {
  const std::size_t index = std::min(drawCount_, numbers_.size() - 1);
  ++drawCount_;
  return numbers_.at(index);
}

std::size_t ScriptedRouletteDraw::drawCount() const { return drawCount_; }

}  // namespace rtype::server::testing
