#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "IRouletteDraw.hpp"

namespace rtype::network::testing {

/**
 * @brief A roulette draw that gives the numbers it was told, in order, then the
 * last one again, and counts the numbers asked of it: the only way to make a
 * round of a test come out on a known number.
 */
class ScriptedRouletteDraw final : public IRouletteDraw {
 public:
  /** @param numbers The numbers to give, in order; must not be empty. */
  explicit ScriptedRouletteDraw(std::vector<std::uint8_t> numbers);

  [[nodiscard]] std::uint8_t drawNumber() override;

  /** @returns How many numbers were asked so far. */
  [[nodiscard]] std::size_t drawCount() const;

 private:
  std::vector<std::uint8_t> numbers_;
  std::size_t drawCount_{0};
};

}  // namespace rtype::network::testing
