#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief The kinds of bet of a single zero roulette. An inside bet covers the
 * numbers that follow its first number on the cloth: Split the next number of
 * its street (n and n + 1), SplitAcross the same place of the next street (n
 * and n + 3). An outside bet names its dozen or column by its first number and
 * its chance by its kind alone.
 */
enum class RouletteBetKind : std::uint8_t {
  Straight = 0,
  Split = 1,
  SplitAcross = 2,
  Street = 3,
  Corner = 4,
  SixLine = 5,
  Dozen = 6,
  Column = 7,
  Red = 8,
  Black = 9,
  Even = 10,
  Odd = 11,
  Low = 12,
  High = 13,
};

}  // namespace rtype::network
