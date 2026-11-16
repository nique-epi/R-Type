#include "MasterCasinoClient.hpp"
#include <httplib.h>
#include <cstdint>
#include <string>
#include "BetsReceipt.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "MasterMessageText.hpp"
#include "MasterRequest.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"
#include "RoundBets.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::network {

MasterCasinoClient::MasterCasinoClient(const std::string& url)
    : transport_(url) {}

CasinoRoomListing MasterCasinoClient::listRooms() {
  return casinoRoomListingFromText(
      transport_.send(masterRequestOf(LIST_CASINO_ROOMS_ROUTE,
                                      apiPathOf(LIST_CASINO_ROOMS_ROUTE)),
                      std::string{}));
}

CasinoRoomState MasterCasinoClient::readRoom(std::uint8_t room) {
  return casinoRoomStateFromText(
      transport_.send(masterRequestOf(READ_CASINO_ROOM_ROUTE,
                                      roomPathOf(READ_CASINO_ROOM_ROUTE, room)),
                      std::string{}));
}

CasinoRoomState MasterCasinoClient::takeSeat(const std::string& token,
                                             std::uint8_t room) {
  return casinoRoomStateFromText(
      transport_.send(masterRequestOf(TAKE_OWN_SEAT_ROUTE,
                                      roomPathOf(TAKE_OWN_SEAT_ROUTE, room)),
                      token));
}

void MasterCasinoClient::leaveSeat(const std::string& token,
                                   std::uint8_t room) {
  static_cast<void>(
      transport_.send(masterRequestOf(LEAVE_OWN_SEAT_ROUTE,
                                      roomPathOf(LEAVE_OWN_SEAT_ROUTE, room)),
                      token));
}

BetsReceipt MasterCasinoClient::placeBets(const std::string& token,
                                          RoomRound round,
                                          const RouletteBets& bets) {
  return betsReceiptFromText(
      transport_.send(jsonRequestOf(PLACE_OWN_BETS_ROUTE,
                                    roundPathOf(PLACE_OWN_BETS_ROUTE, round),
                                    rouletteBetsText(bets)),
                      token));
}

RoundBets MasterCasinoClient::readOwnBets(const std::string& token,
                                          RoomRound round) {
  return roundBetsFromText(
      transport_.send(masterRequestOf(READ_OWN_BETS_ROUTE,
                                      roundPathOf(READ_OWN_BETS_ROUTE, round)),
                      token));
}

}  // namespace rtype::network
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
