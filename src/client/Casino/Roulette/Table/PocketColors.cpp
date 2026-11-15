#include "PocketColors.hpp"
#include <algorithm>
#include <cstdint>
#include "PocketShades.hpp"
#include "ProtocolConstants.hpp"
#include "RouletteColorConstants.hpp"
#include "RouletteScreenConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether the pocket of @p number is red. */
bool isRed(std::uint8_t number) {
  return std::ranges::find(network::RED_ROULETTE_NUMBERS, number) !=
         network::RED_ROULETTE_NUMBERS.end();
}

}  // namespace

PocketShades pocketShadesOf(std::uint8_t number) {
  if (number == 0) {
    return GREEN_POCKET_SHADES;
  }
  return isRed(number) ? RED_POCKET_SHADES : BLACK_POCKET_SHADES;
}

const char* pocketColorWordOf(std::uint8_t number) {
  if (number == 0) {
    return GREEN_WORD;
  }
  return isRed(number) ? RED_WORD : BLACK_WORD;
}

}  // namespace rtype::client
