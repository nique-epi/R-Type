#include "RoundReference.hpp"
#include <string>
#include "CasinoHelpersConstants.hpp"
#include "RoomRound.hpp"

namespace rtype::master {

std::string roundReferenceOf(network::RoomRound place) {
  return std::string{ROUND_REFERENCE_PREFIX} + std::to_string(place.room) +
         std::string{ROUND_REFERENCE_SEPARATOR} + std::to_string(place.round);
}

}  // namespace rtype::master
