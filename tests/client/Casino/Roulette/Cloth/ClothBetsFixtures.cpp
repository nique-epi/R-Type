#include "ClothBetsFixtures.hpp"
#include <algorithm>
#include <cstddef>
#include <vector>
#include "ClothBetsTestConstants.hpp"
#include "ClothConstants.hpp"
#include "ClothDirection.hpp"
#include "ClothMoves.hpp"
#include "ClothPlace.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

namespace rtype::client::testing {

namespace {

using network::RouletteBetKind;

/** @returns Whether the @p count numbers from @p first on stay on the
 * cloth. */
bool staysOnCloth(unsigned int first, unsigned int count) {
  return first >= 1 && first + count - 1 <= HIGHEST_NUMBER;
}

/** @returns Whether @p first opens a street. */
bool opensStreet(unsigned int first) {
  return first >= 1 && (first - 1) % STREET_LENGTH == 0;
}

/** @returns Whether @p first has a next number in its street. */
bool hasNextInStreet(unsigned int first) {
  return first >= 1 && ((first - 1) % STREET_LENGTH) + 1 < STREET_LENGTH;
}

}  // namespace

std::vector<ClothPlace> reachableClothPlaces() {
  std::vector<ClothPlace> reached{FIRST_CLOTH_PLACE};
  for (std::size_t next = 0; next < reached.size(); ++next) {
    for (const ClothDirection direction :
         {ClothDirection::Left, ClothDirection::Right, ClothDirection::Up,
          ClothDirection::Down}) {
      const ClothPlace place = placeAfter(reached.at(next), direction);
      if (std::ranges::find(reached, place) == reached.end()) {
        reached.push_back(place);
      }
    }
  }
  return reached;
}

bool serverTakes(const network::RouletteBet& bet) {
  const unsigned int first = bet.firstNumber;
  switch (bet.kind) {
    case RouletteBetKind::Straight:
      return first <= HIGHEST_NUMBER;
    case RouletteBetKind::Split:
      return hasNextInStreet(first) && staysOnCloth(first, 2);
    case RouletteBetKind::SplitAcross:
      return staysOnCloth(first, STREET_LENGTH + 1);
    case RouletteBetKind::Street:
      return opensStreet(first) && staysOnCloth(first, STREET_LENGTH);
    case RouletteBetKind::Corner:
      return hasNextInStreet(first) && staysOnCloth(first, STREET_LENGTH + 2);
    case RouletteBetKind::SixLine:
      return opensStreet(first) && staysOnCloth(first, 2 * STREET_LENGTH);
    case RouletteBetKind::Dozen:
      return std::ranges::find(DOZEN_FIRSTS, first) != DOZEN_FIRSTS.end();
    case RouletteBetKind::Column:
      return first >= 1 && first <= STREET_LENGTH;
    default:
      break;
  }
  return first == 0;
}

}  // namespace rtype::client::testing
