#include "ClothMoves.hpp"
#include <algorithm>
#include "ClothBand.hpp"
#include "ClothConstants.hpp"
#include "ClothDirection.hpp"
#include "ClothPlace.hpp"

namespace rtype::client {

namespace {

/** @returns Whether the columns, which have no line between them, take the
 * cursor at @p down. */
bool columnsTake(int down) {
  return down % ROW_STEP_DOWN == 0 && down < STREET_EDGE_DOWN;
}

/** @returns @p place one step across the numbers, towards the columns when
 * @p step is 1, towards the zero when it is -1. */
ClothPlace acrossTheNumbers(ClothPlace place, int step) {
  const int across = place.across + step;
  if (across < ZERO_ACROSS || across > NUMBERS_LAST_ACROSS ||
      (across == NUMBERS_LAST_ACROSS && !columnsTake(place.down))) {
    return place;
  }
  return {.band = ClothBand::Numbers, .across = across, .down = place.down};
}

/** @returns The dozen under @p across of the band of the numbers. */
int dozenUnder(int across) {
  return std::clamp((across - 1) / ACROSS_PER_DOZEN, 0, CLOTH_DOZENS - 1);
}

/** @returns @p place one step down the numbers, the street edge giving way to
 * the dozen under it. */
ClothPlace downTheNumbers(ClothPlace place) {
  if (place.across == ZERO_ACROSS) {
    return place;
  }
  const int step = place.across == NUMBERS_LAST_ACROSS ? ROW_STEP_DOWN : 1;
  if (place.down + step > STREET_EDGE_DOWN ||
      (step == ROW_STEP_DOWN && !columnsTake(place.down + step))) {
    return {.band = ClothBand::Dozens,
            .across = dozenUnder(place.across),
            .down = 0};
  }
  return {.band = ClothBand::Numbers,
          .across = place.across,
          .down = place.down + step};
}

/** @returns @p place one step up the numbers; the top row stays. */
ClothPlace upTheNumbers(ClothPlace place) {
  const int step = place.across == NUMBERS_LAST_ACROSS ? ROW_STEP_DOWN : 1;
  if (place.across == ZERO_ACROSS || place.down - step < 0) {
    return place;
  }
  return {.band = ClothBand::Numbers,
          .across = place.across,
          .down = place.down - step};
}

/** @returns @p place in the band of the numbers moved towards
 * @p direction. */
ClothPlace onTheNumbers(ClothPlace place, ClothDirection direction) {
  switch (direction) {
    case ClothDirection::Left:
      return acrossTheNumbers(place, -1);
    case ClothDirection::Right:
      return acrossTheNumbers(place, 1);
    case ClothDirection::Up:
      return upTheNumbers(place);
    case ClothDirection::Down:
      break;
  }
  return downTheNumbers(place);
}

/** @returns @p place, in a band of @p count outside bets, one step across
 * towards @p step; the ends stay. */
ClothPlace alongTheBand(ClothPlace place, int step, int count) {
  return {.band = place.band,
          .across = std::clamp(place.across + step, 0, count - 1),
          .down = 0};
}

/** @returns @p place among the dozens moved towards @p direction: up to the
 * first street of the dozen, down to its first even chance. */
ClothPlace onTheDozens(ClothPlace place, ClothDirection direction) {
  switch (direction) {
    case ClothDirection::Left:
      return alongTheBand(place, -1, CLOTH_DOZENS);
    case ClothDirection::Right:
      return alongTheBand(place, 1, CLOTH_DOZENS);
    case ClothDirection::Up:
      return {.band = ClothBand::Numbers,
              .across = 1 + (place.across * ACROSS_PER_DOZEN),
              .down = STREET_EDGE_DOWN};
    case ClothDirection::Down:
      break;
  }
  return {.band = ClothBand::EvenChances,
          .across = place.across * EVEN_CHANCES_PER_DOZEN,
          .down = 0};
}

/** @returns @p place among the even chances moved towards @p direction: up
 * to the dozen over it; down stays. */
ClothPlace onTheEvenChances(ClothPlace place, ClothDirection direction) {
  const int count = CLOTH_DOZENS * EVEN_CHANCES_PER_DOZEN;
  switch (direction) {
    case ClothDirection::Left:
      return alongTheBand(place, -1, count);
    case ClothDirection::Right:
      return alongTheBand(place, 1, count);
    case ClothDirection::Up:
      return {.band = ClothBand::Dozens,
              .across = place.across / EVEN_CHANCES_PER_DOZEN,
              .down = 0};
    case ClothDirection::Down:
      break;
  }
  return place;
}

}  // namespace

ClothPlace placeAfter(ClothPlace place, ClothDirection direction) {
  switch (place.band) {
    case ClothBand::Numbers:
      return onTheNumbers(place, direction);
    case ClothBand::Dozens:
      return onTheDozens(place, direction);
    case ClothBand::EvenChances:
      break;
  }
  return onTheEvenChances(place, direction);
}

}  // namespace rtype::client
