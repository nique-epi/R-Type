#include "CasinoRoutesFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include "CasinoRoutesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "MasterTestConstants.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteBets.hpp"
#include "RouteCall.hpp"
#include "UserCredentials.hpp"

namespace {

constexpr std::uint8_t LUCKY_SEVEN = 7;

}  // namespace

rtype::network::UserCredentials secondPilotCredentials() {
  return rtype::network::UserCredentials{
      .nickname = std::string{SECOND_PILOT_NICKNAME},
      .password = std::string{SECOND_PILOT_PASSWORD}};
}

std::string ownBetsPathOf(std::uint64_t round) {
  return rtype::network::roundPathOf(
      rtype::network::PLACE_OWN_BETS_ROUTE,
      rtype::network::RoomRound{.room = FIRST_ROOM, .round = round});
}

RouteCall betsCall(std::string_view path, std::string_view token,
                   std::string_view body) {
  return RouteCall{.method = PUT_METHOD,
                   .path = path,
                   .key = token,
                   .body = std::string{body},
                   .overHttps = true};
}

std::string straightBetsOf(std::size_t count) {
  rtype::network::RouletteBets bets;
  for (std::size_t index = 0; index < count; ++index) {
    bets.bets.push_back(rtype::network::RouletteBet{
        .kind = rtype::network::RouletteBetKind::Straight,
        .firstNumber = LUCKY_SEVEN,
        .amount = ONE_CHIP});
  }
  return rtype::network::rouletteBetsText(bets);
}
