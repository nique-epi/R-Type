#include "RouletteRules.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteConstants.hpp"

namespace rtype::network {

namespace {

/** @returns The mask holding @p count numbers from @p first on, one bit per
 * number of the wheel. */
constexpr std::uint64_t runOf(unsigned int first, unsigned int count) {
  return ((std::uint64_t{1} << count) - 1) << first;
}

/** @returns The mask of every number of the cloth, the zero left out. */
constexpr std::uint64_t CLOTH_MASK =
    runOf(LOWEST_CLOTH_NUMBER, HIGHEST_ROULETTE_NUMBER);

/** @returns The mask of the numbers @p keep says yes to, the zero left out. */
template <typename Keep>
constexpr std::uint64_t maskOf(Keep keep) {
  std::uint64_t mask = 0;
  for (unsigned int number = LOWEST_CLOTH_NUMBER;
       number <= HIGHEST_ROULETTE_NUMBER; ++number) {
    if (keep(number)) {
      mask |= std::uint64_t{1} << number;
    }
  }
  return mask;
}

constexpr std::uint64_t RED_MASK = maskOf([](unsigned int number) {
  return std::ranges::find(RED_ROULETTE_NUMBERS, number) !=
         RED_ROULETTE_NUMBERS.end();
});
constexpr std::uint64_t EVEN_MASK =
    maskOf([](unsigned int number) { return number % 2 == 0; });
constexpr std::uint64_t FIRST_COLUMN_MASK = maskOf([](unsigned int number) {
  return (number - LOWEST_CLOTH_NUMBER) % NUMBERS_PER_STREET == 0;
});

/** @returns Where @p number sits in its street, 0 to 2. */
constexpr unsigned int placeInStreet(unsigned int number) {
  return (number - LOWEST_CLOTH_NUMBER) % NUMBERS_PER_STREET;
}

/** @returns Whether @p number is on the cloth and opens a street. */
bool opensStreet(unsigned int number) {
  return number >= LOWEST_CLOTH_NUMBER && placeInStreet(number) == 0;
}

/** @returns Whether @p number is on the cloth and has a neighbour after it in
 * its street. */
bool hasNextInStreet(unsigned int number) {
  return number >= LOWEST_CLOTH_NUMBER &&
         placeInStreet(number) + 1 < NUMBERS_PER_STREET;
}

/** @returns Whether the @p count numbers from @p first on stay on the cloth. */
bool fitsOnCloth(unsigned int first, unsigned int count) {
  return first + count - 1 <= HIGHEST_ROULETTE_NUMBER;
}

/** @returns The numbers @p bet covers, one bit per number of the wheel; the
 * bet is placeable. */
std::uint64_t coverageOf(const RouletteBet& bet) {
  const unsigned int first = bet.firstNumber;
  switch (bet.kind) {
    case RouletteBetKind::Straight:
      return runOf(first, 1);
    case RouletteBetKind::Split:
      return runOf(first, 2);
    case RouletteBetKind::SplitAcross:
      return runOf(first, 1) | runOf(first + NUMBERS_PER_STREET, 1);
    case RouletteBetKind::Street:
      return runOf(first, NUMBERS_PER_STREET);
    case RouletteBetKind::Corner:
      return runOf(first, 2) | runOf(first + NUMBERS_PER_STREET, 2);
    case RouletteBetKind::SixLine:
      return runOf(first, 2 * NUMBERS_PER_STREET);
    case RouletteBetKind::Dozen:
      return runOf(first, NUMBERS_PER_DOZEN);
    case RouletteBetKind::Column:
      return FIRST_COLUMN_MASK << placeInStreet(first);
    case RouletteBetKind::Red:
      return RED_MASK;
    case RouletteBetKind::Black:
      return CLOTH_MASK & ~RED_MASK;
    case RouletteBetKind::Even:
      return EVEN_MASK;
    case RouletteBetKind::Odd:
      return CLOTH_MASK & ~EVEN_MASK;
    case RouletteBetKind::Low:
      return runOf(LOWEST_CLOTH_NUMBER, LAST_LOW_NUMBER);
    case RouletteBetKind::High:
      return runOf(LAST_LOW_NUMBER + 1, LAST_LOW_NUMBER);
  }
  return 0;
}

/** @returns What a winning bet of @p kind returns per chip, its stake
 * included. */
std::uint32_t returnOf(RouletteBetKind kind) {
  switch (kind) {
    case RouletteBetKind::Straight:
      return STRAIGHT_RETURN;
    case RouletteBetKind::Split:
    case RouletteBetKind::SplitAcross:
      return SPLIT_RETURN;
    case RouletteBetKind::Street:
      return STREET_RETURN;
    case RouletteBetKind::Corner:
      return CORNER_RETURN;
    case RouletteBetKind::SixLine:
      return SIX_LINE_RETURN;
    case RouletteBetKind::Dozen:
    case RouletteBetKind::Column:
      return DOZEN_RETURN;
    case RouletteBetKind::Red:
    case RouletteBetKind::Black:
    case RouletteBetKind::Even:
    case RouletteBetKind::Odd:
    case RouletteBetKind::Low:
    case RouletteBetKind::High:
      return EVEN_CHANCE_RETURN;
  }
  return 0;
}

}  // namespace

bool isPlaceableBet(const RouletteBet& bet) {
  const unsigned int first = bet.firstNumber;
  switch (bet.kind) {
    case RouletteBetKind::Straight:
      return first <= HIGHEST_ROULETTE_NUMBER;
    case RouletteBetKind::Split:
      return hasNextInStreet(first) && fitsOnCloth(first, 2);
    case RouletteBetKind::SplitAcross:
      return first >= LOWEST_CLOTH_NUMBER &&
             fitsOnCloth(first, NUMBERS_PER_STREET + 1);
    case RouletteBetKind::Street:
      return opensStreet(first) && fitsOnCloth(first, NUMBERS_PER_STREET);
    case RouletteBetKind::Corner:
      return hasNextInStreet(first) &&
             fitsOnCloth(first, NUMBERS_PER_STREET + 2);
    case RouletteBetKind::SixLine:
      return opensStreet(first) && fitsOnCloth(first, 2 * NUMBERS_PER_STREET);
    case RouletteBetKind::Dozen:
      return first >= LOWEST_CLOTH_NUMBER &&
             (first - LOWEST_CLOTH_NUMBER) % NUMBERS_PER_DOZEN == 0 &&
             fitsOnCloth(first, NUMBERS_PER_DOZEN);
    case RouletteBetKind::Column:
      return first >= LOWEST_CLOTH_NUMBER && first <= NUMBERS_PER_STREET;
    case RouletteBetKind::Red:
    case RouletteBetKind::Black:
    case RouletteBetKind::Even:
    case RouletteBetKind::Odd:
    case RouletteBetKind::Low:
    case RouletteBetKind::High:
      return first == 0;
  }
  return false;
}

std::uint64_t payoutOf(const RouletteBet& bet, std::uint8_t drawn) {
  if (drawn > HIGHEST_ROULETTE_NUMBER || !isPlaceableBet(bet) ||
      ((coverageOf(bet) >> drawn) & 1U) == 0) {
    return 0;
  }
  return static_cast<std::uint64_t>(bet.amount) * returnOf(bet.kind);
}

std::uint64_t stakeOf(const std::vector<RouletteBet>& bets) {
  std::uint64_t stake = 0;
  for (const RouletteBet& bet : bets) {
    stake += bet.amount;
  }
  return stake;
}

bool fitsRoundCaps(const std::vector<RouletteBet>& bets) {
  return bets.size() <= MAXIMUM_ROULETTE_BETS &&
         stakeOf(bets) <= MAXIMUM_ROUND_STAKE;
}

}  // namespace rtype::network
