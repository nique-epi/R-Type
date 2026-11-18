#include "ClothBets.hpp"
#include <cstddef>
#include <cstdint>
#include "ClothBand.hpp"
#include "ClothConstants.hpp"
#include "ClothPlace.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteConstants.hpp"

namespace rtype::client {

namespace {

using network::NUMBERS_PER_DOZEN;
using network::NUMBERS_PER_STREET;
using network::RouletteBet;
using network::RouletteBetKind;

/** @returns The number of street @p street, row @p down of the band of the
 * numbers, the top row first. */
std::uint8_t numberAt(int street, int down) {
  return static_cast<std::uint8_t>((NUMBERS_PER_STREET * street) +
                                   NUMBERS_PER_STREET - (down / ROW_STEP_DOWN));
}

/** @returns A bet of @p kind from @p firstNumber, its amount 0. */
RouletteBet betOf(RouletteBetKind kind, std::uint8_t firstNumber) {
  return RouletteBet{.kind = kind, .firstNumber = firstNumber, .amount = 0};
}

/** @returns The bet on a column of numbers, @p street, at @p down. */
RouletteBet betOnStreet(int street, int down) {
  if (down == STREET_EDGE_DOWN) {
    return betOf(RouletteBetKind::Street, numberAt(street, STREET_EDGE_DOWN));
  }
  if (down % ROW_STEP_DOWN == 0) {
    return betOf(RouletteBetKind::Straight, numberAt(street, down));
  }
  return betOf(RouletteBetKind::Split, numberAt(street, down + 1));
}

/** @returns The bet on the line after street @p street, at @p down. */
RouletteBet betBetweenStreets(int street, int down) {
  if (down == STREET_EDGE_DOWN) {
    return betOf(RouletteBetKind::SixLine, numberAt(street, STREET_EDGE_DOWN));
  }
  if (down % ROW_STEP_DOWN == 0) {
    return betOf(RouletteBetKind::SplitAcross, numberAt(street, down));
  }
  return betOf(RouletteBetKind::Corner, numberAt(street, down + 1));
}

/** @returns The bet on @p place of the band of the numbers. */
RouletteBet betOnNumbers(ClothPlace place) {
  if (place.across == ZERO_ACROSS) {
    return betOf(RouletteBetKind::Straight, 0);
  }
  if (place.across == NUMBERS_LAST_ACROSS) {
    return betOf(RouletteBetKind::Column, numberAt(0, place.down));
  }
  if (place.across % 2 == 1) {
    return betOnStreet((place.across - 1) / 2, place.down);
  }
  return betBetweenStreets((place.across / 2) - 1, place.down);
}

/** @returns The step down of the row of @p number, the top row 0. */
int downOf(std::uint8_t number) {
  return ROW_STEP_DOWN *
         (NUMBERS_PER_STREET - 1 - ((number - 1) % NUMBERS_PER_STREET));
}

/** @returns The step across of the street of @p number. */
int acrossOf(std::uint8_t number) {
  return (2 * ((number - 1) / NUMBERS_PER_STREET)) + 1;
}

/** @returns The place of @p kind, one of the even chances. */
ClothPlace placeOfEvenChance(RouletteBetKind kind) {
  int across = 0;
  while (static_cast<std::size_t>(across) < EVEN_CHANCE_KINDS.size() &&
         EVEN_CHANCE_KINDS.at(static_cast<std::size_t>(across)) != kind) {
    ++across;
  }
  return {.band = ClothBand::EvenChances, .across = across, .down = 0};
}

/** @returns The place of @p bet, an inside bet on a number other than the
 * zero. */
ClothPlace placeOfInside(const RouletteBet& bet) {
  const std::uint8_t first = bet.firstNumber;
  switch (bet.kind) {
    case RouletteBetKind::Split:
      return {.across = acrossOf(first), .down = downOf(first) - 1};
    case RouletteBetKind::SplitAcross:
      return {.across = acrossOf(first) + 1, .down = downOf(first)};
    case RouletteBetKind::Street:
      return {.across = acrossOf(first), .down = STREET_EDGE_DOWN};
    case RouletteBetKind::Corner:
      return {.across = acrossOf(first) + 1, .down = downOf(first) - 1};
    case RouletteBetKind::SixLine:
      return {.across = acrossOf(first) + 1, .down = STREET_EDGE_DOWN};
    default:
      break;
  }
  return {.across = acrossOf(first), .down = downOf(first)};
}

}  // namespace

RouletteBet betAt(ClothPlace place) {
  switch (place.band) {
    case ClothBand::Numbers:
      return betOnNumbers(place);
    case ClothBand::Dozens:
      return betOf(
          RouletteBetKind::Dozen,
          static_cast<std::uint8_t>(1 + (NUMBERS_PER_DOZEN * place.across)));
    case ClothBand::EvenChances:
      break;
  }
  return betOf(EVEN_CHANCE_KINDS.at(static_cast<std::size_t>(place.across)), 0);
}

ClothPlace placeOf(const RouletteBet& bet) {
  switch (bet.kind) {
    case RouletteBetKind::Straight:
      if (bet.firstNumber == 0) {
        return {.across = ZERO_ACROSS, .down = ROW_STEP_DOWN};
      }
      return placeOfInside(bet);
    case RouletteBetKind::Column:
      return {.across = NUMBERS_LAST_ACROSS, .down = downOf(bet.firstNumber)};
    case RouletteBetKind::Dozen:
      return {.band = ClothBand::Dozens,
              .across = (bet.firstNumber - 1) / NUMBERS_PER_DOZEN,
              .down = 0};
    case RouletteBetKind::Red:
    case RouletteBetKind::Black:
    case RouletteBetKind::Even:
    case RouletteBetKind::Odd:
    case RouletteBetKind::Low:
    case RouletteBetKind::High:
      return placeOfEvenChance(bet.kind);
    default:
      break;
  }
  return placeOfInside(bet);
}

}  // namespace rtype::client
