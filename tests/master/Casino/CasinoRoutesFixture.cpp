#include "CasinoRoutesFixture.hpp"
#include <httplib.h>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "CasinoRoutesFixtures.hpp"
#include "CasinoRoutesTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "UserCredentials.hpp"
#include "WalletRoutesFixtures.hpp"
#include "WalletRoutesTestConstants.hpp"

namespace {

/** @returns The token of a session of the account @p credentials creates on
 * the master of @p port. */
std::string tokenOfNewAccount(
    std::uint16_t port, const rtype::network::UserCredentials& credentials) {
  static_cast<void>(
      sendRouteCall(port, credentialsCall(USERS_URL, credentials)));
  return memberIn(
      sendRouteCall(port, credentialsCall(SESSIONS_URL, credentials)),
      TOKEN_MEMBER_NAME);
}

/** @returns The round of @p room when its bets stay open BETS_OPEN_MARGIN
 * more at least, nothing otherwise. */
std::optional<std::uint64_t> roundOpenLongEnough(const nlohmann::json& room) {
  if (!room.is_object() || room.value(std::string{PHASE_MEMBER_NAME},
                                      std::string{}) != BETTING_PHASE_TEXT) {
    return std::nullopt;
  }
  const auto left =
      room.at(std::string{CLOSES_AT_MEMBER_NAME}).get<std::int64_t>() -
      room.at(std::string{SERVER_TIME_MEMBER_NAME}).get<std::int64_t>();
  if (left < BETS_OPEN_MARGIN.count()) {
    return std::nullopt;
  }
  return room.at(std::string{ROUND_MEMBER_NAME}).get<std::uint64_t>();
}

}  // namespace

CasinoRoutesFixture::CasinoRoutesFixture()
    : master_(std::vector<std::string>{
          std::string{TWO_ROOMS_LINE}, std::string{ONE_SEAT_LINE},
          std::string{LONG_BETS_LINE}, std::string{SHORT_SPIN_LINE},
          std::string{SHORT_RESULT_LINE}}),
      ace_(tokenOfNewAccount(master_.port(), aceCredentials())),
      secondPilot_(
          tokenOfNewAccount(master_.port(), secondPilotCredentials())) {}

const std::string& CasinoRoutesFixture::ace() const { return ace_; }

const std::string& CasinoRoutesFixture::secondPilot() const {
  return secondPilot_;
}

httplib::Result CasinoRoutesFixture::send(const RouteCall& call) const {
  return sendRouteCall(master_.port(), call);
}

httplib::Result CasinoRoutesFixture::takeSeat(std::string_view token) const {
  return send(ownCall(PUT_METHOD, FIRST_SEAT_URL, token));
}

std::optional<std::uint64_t> CasinoRoutesFixture::openRound() const {
  for (int poll = 0; poll < OPEN_BETS_POLLS; ++poll) {
    const httplib::Result room =
        send(ownCall(GET_METHOD_TEXT, FIRST_ROOM_URL, {}));
    if (room) {
      if (const std::optional<std::uint64_t> round = roundOpenLongEnough(
              nlohmann::json::parse(room->body, nullptr, false))) {
        return round;
      }
    }
    std::this_thread::sleep_for(OPEN_BETS_POLL);
  }
  return std::nullopt;
}

std::optional<std::int64_t> CasinoRoutesFixture::chipsOf(
    std::string_view token) const {
  return numberIn(send(ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, token)),
                  CHIPS_MEMBER_NAME);
}
