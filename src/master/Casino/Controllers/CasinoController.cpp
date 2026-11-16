#include "CasinoController.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "BetOrder.hpp"
#include "BetRefusal.hpp"
#include "BetsReceipt.hpp"
#include "CasinoControllerConstants.hpp"
#include "CasinoRoomConstants.hpp"
#include "CasinoService.hpp"
#include "DatedRequest.hpp"
#include "HttpConstants.hpp"
#include "ISessionAccounts.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MillisecondDate.hpp"
#include "QueryBounds.hpp"
#include "RoomRound.hpp"
#include "RouletteSchedule.hpp"
#include "RouteAnswers.hpp"
#include "SeatKey.hpp"
#include "SeatOutcome.hpp"
#include "User.hpp"

namespace rtype::master {

namespace {

network::ApiResponse okResponse(std::string body) {
  return network::ApiResponse{.status = OK_STATUS, .body = std::move(body)};
}

network::ApiResponse unknownRoomRefusal() {
  return refusal(NOT_FOUND_STATUS, UNKNOWN_ROOM_DETAIL);
}

/** @returns The number of a room the path of @p request names, whether the
 * casino runs it or not; nothing when it is no number of a room. */
std::optional<std::uint8_t> roomNumberOf(const network::ApiRequest& request) {
  const std::optional<std::string> text =
      pathTextOf(request, network::ROOM_PATH_PARAMETER);
  if (!text.has_value()) {
    return std::nullopt;
  }
  const std::optional<std::int64_t> room = boundedNumberIn(
      *text, QueryBounds{.lowest = network::FIRST_CASINO_ROOM,
                         .highest = network::MAXIMUM_CASINO_ROOMS,
                         .absent = network::FIRST_CASINO_ROOM});
  if (!room.has_value()) {
    return std::nullopt;
  }
  return static_cast<std::uint8_t>(*room);
}

/** @returns The room the path of @p request names, nothing when it names no
 * room @p service runs. */
std::optional<std::uint8_t> roomOf(const network::ApiRequest& request,
                                   const CasinoService& service) {
  const std::optional<std::uint8_t> room = roomNumberOf(request);
  if (!room.has_value() || !service.hasRoom(*room)) {
    return std::nullopt;
  }
  return room;
}

/** @returns The room and the round the path of @p request names, nothing
 * when it names no room @p service runs, or a round that is no number. */
std::optional<network::RoomRound> roomRoundOf(
    const network::ApiRequest& request, const CasinoService& service) {
  const std::optional<std::uint8_t> room = roomOf(request, service);
  const std::optional<std::string> text =
      pathTextOf(request, network::ROUND_PATH_PARAMETER);
  if (!room.has_value() || !text.has_value()) {
    return std::nullopt;
  }
  const std::optional<std::int64_t> round =
      boundedNumberIn(*text, QueryBounds{.lowest = FIRST_ROUND,
                                         .highest = LAST_ROUND,
                                         .absent = FIRST_ROUND});
  if (!round.has_value()) {
    return std::nullopt;
  }
  return network::RoomRound{.room = *room,
                            .round = static_cast<std::uint64_t>(*round)};
}

network::ApiResponse betRefusalOf(BetRefusal reason) {
  switch (reason) {
    case BetRefusal::BetsClosed:
      return refusal(CONFLICT_STATUS, BETS_CLOSED_DETAIL);
    case BetRefusal::NotSeated:
      return refusal(CONFLICT_STATUS, NOT_SEATED_DETAIL);
    case BetRefusal::InvalidBet:
      return refusal(UNPROCESSABLE_CONTENT_STATUS, INVALID_BET_DETAIL);
    case BetRefusal::TooManyBets:
      return refusal(UNPROCESSABLE_CONTENT_STATUS, TOO_MANY_BETS_DETAIL);
    case BetRefusal::OverTheCap:
      return refusal(UNPROCESSABLE_CONTENT_STATUS, OVER_THE_CAP_DETAIL);
    case BetRefusal::NotEnoughChips:
      return refusal(UNPROCESSABLE_CONTENT_STATUS, NOT_ENOUGH_CHIPS_DETAIL);
  }
  return refusal(CONFLICT_STATUS, BETS_CLOSED_DETAIL);
}

network::ApiResponse betsResponseOf(
    const std::variant<network::BetsReceipt, BetRefusal>& outcome) {
  if (const auto* receipt = std::get_if<network::BetsReceipt>(&outcome)) {
    return okResponse(network::betsReceiptText(*receipt));
  }
  return betRefusalOf(std::get<BetRefusal>(outcome));
}

DatedRequest datedNow(const network::ApiRequest& request) {
  return DatedRequest{.request = std::cref(request), .now = currentSecond()};
}

}  // namespace

CasinoController::CasinoController(ISessionAccounts& accounts,
                                   CasinoService& service)
    : accounts_(accounts), service_(service) {}

network::ApiResponse CasinoController::listRooms(
    const network::ApiRequest& /*request*/) {
  return okResponse(
      network::casinoRoomListingText(service_.rooms(currentMillisecond())));
}

network::ApiResponse CasinoController::readRoom(
    const network::ApiRequest& request) {
  const std::optional<std::uint8_t> room = roomOf(request, service_);
  if (!room.has_value()) {
    return unknownRoomRefusal();
  }
  return okResponse(network::casinoRoomStateText(
      service_.roomState(*room, currentMillisecond())));
}

network::ApiResponse CasinoController::takeOwnSeat(
    const network::ApiRequest& request) {
  const DatedRequest dated = datedNow(request);
  return answerAsAccountOf(
      accounts_, dated, [this, &dated](const network::User& user) {
        const std::optional<std::uint8_t> room =
            roomOf(dated.request.get(), service_);
        if (!room.has_value()) {
          return unknownRoomRefusal();
        }
        const SeatOutcome outcome = service_.takeSeat(
            SeatKey{.room = *room, .userId = user.id}, dated.now);
        if (outcome == SeatOutcome::RoomFull) {
          return refusal(CONFLICT_STATUS, ROOM_FULL_DETAIL);
        }
        return network::ApiResponse{
            .status =
                outcome == SeatOutcome::Taken ? CREATED_STATUS : OK_STATUS,
            .body = network::casinoRoomStateText(
                service_.roomState(*room, currentMillisecond()))};
      });
}

network::ApiResponse CasinoController::leaveOwnSeat(
    const network::ApiRequest& request) {
  const DatedRequest dated = datedNow(request);
  return answerAsAccountOf(
      accounts_, dated, [this, &dated](const network::User& user) {
        const std::optional<std::uint8_t> room =
            roomNumberOf(dated.request.get());
        if (room.has_value()) {
          service_.leaveSeat(SeatKey{.room = *room, .userId = user.id});
        }
        return network::ApiResponse{.status = NO_CONTENT_STATUS, .body = {}};
      });
}

network::ApiResponse CasinoController::placeOwnBets(
    const network::ApiRequest& request) {
  const DatedRequest dated = datedNow(request);
  return answerAsAccountOf(
      accounts_, dated, [this, &dated](const network::User& user) {
        const std::optional<network::RoomRound> place =
            roomRoundOf(dated.request.get(), service_);
        if (!place.has_value()) {
          return unknownRoomRefusal();
        }
        return betsResponseOf(service_.placeBets(BetOrder{
            .place = *place,
            .userId = user.id,
            .bets =
                network::rouletteBetsFromText(dated.request.get().body).bets,
            .sentAt = currentMillisecond()}));
      });
}

network::ApiResponse CasinoController::readOwnBets(
    const network::ApiRequest& request) {
  const DatedRequest dated = datedNow(request);
  return answerAsAccountOf(
      accounts_, dated, [this, &dated](const network::User& user) {
        const std::optional<network::RoomRound> place =
            roomRoundOf(dated.request.get(), service_);
        if (!place.has_value()) {
          return unknownRoomRefusal();
        }
        return okResponse(
            network::roundBetsText(service_.betsOf(*place, user.id)));
      });
}

}  // namespace rtype::master
