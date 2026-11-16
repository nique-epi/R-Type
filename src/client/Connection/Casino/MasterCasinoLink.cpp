#include "MasterCasinoLink.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include "BetsReading.hpp"
#include "CasinoAsks.hpp"
#include "CasinoLinkConstants.hpp"
#include "CasinoLinkState.hpp"
#include "CasinoReadings.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRefusalConstants.hpp"
#include "CasinoRoomReading.hpp"
#include "CasinoRoomsReading.hpp"
#include "CasinoSeat.hpp"
#include "HttpStatusConstants.hpp"
#include "MasterCasinoClient.hpp"
#include "NetworkException.hpp"
#include "OwnBetsReading.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"
#include "SeatReading.hpp"

namespace rtype::client {

namespace {

/** @returns Why the master refused a seat with @p status. */
CasinoRefusal seatRefusalOf(int status) {
  switch (status) {
    case network::CONFLICT_STATUS:
      return CasinoRefusal::RoomFull;
    case network::UNAUTHORIZED_STATUS:
      return CasinoRefusal::SignedOut;
    default:
      return CasinoRefusal::MasterRefused;
  }
}

/** @returns Why the master refused bets as @p refused says: its status, and
 * its detail for two refusals of one status. */
CasinoRefusal betsRefusalOf(const network::MasterRefusedException& refused) {
  switch (refused.status()) {
    case network::CONFLICT_STATUS:
      return refused.detail() == network::NOT_SEATED_DETAIL
                 ? CasinoRefusal::NotSeated
                 : CasinoRefusal::BetsClosed;
    case network::UNPROCESSABLE_CONTENT_STATUS:
      return refused.detail() == network::NOT_ENOUGH_CHIPS_DETAIL
                 ? CasinoRefusal::NotEnoughChips
                 : CasinoRefusal::RuleBroken;
    case network::UNAUTHORIZED_STATUS:
      return CasinoRefusal::SignedOut;
    default:
      return CasinoRefusal::MasterRefused;
  }
}

/** @returns Why @p call failed, nothing when it went through; @p refusalOf
 * reads a refusal of the master. */
template <typename Call, typename RefusalOf>
std::optional<CasinoRefusal> failureOf(Call call, RefusalOf refusalOf) {
  try {
    call();
  } catch (const network::MasterRefusedException& refused) {
    return refusalOf(refused);
  } catch (const network::MasterUnreachableException&) {
    return CasinoRefusal::MasterUnreachable;
  } catch (const network::NetworkException&) {
    return CasinoRefusal::MasterRefused;
  }
  return std::nullopt;
}

/** @returns What @p call reads, or nothing when the master does not answer,
 * refuses, or answers what cannot be read. */
template <typename Call>
auto readingOrNothing(Call call) -> std::optional<decltype(call())> {
  try {
    return call();
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
}

/** @brief Takes, renews or leaves the seat of @p asks when the ask is newer
 * than the one @p state served. */
void serveSeat(network::MasterCasinoClient& client, const CasinoAsks& asks,
               CasinoLinkState& state) {
  if (asks.seatAsk <= state.served.seatAsk) {
    return;
  }
  SeatReading reading{.ask = asks.seatAsk, .seat = asks.seat, .refusal = {}};
  reading.refusal = failureOf(
      [&client, &asks] {
        if (asks.seat.seated) {
          static_cast<void>(client.takeSeat(asks.token, asks.seat.room));
        } else {
          client.leaveSeat(asks.token, asks.seat.room);
        }
      },
      [](const network::MasterRefusedException& refused) {
        return seatRefusalOf(refused.status());
      });
  state.seat.publish(reading);
  state.served.seatAsk = asks.seatAsk;
}

/** @brief Lays the bets of @p asks when the ask is newer than the one
 * @p state served. */
void serveBets(network::MasterCasinoClient& client, const CasinoAsks& asks,
               CasinoLinkState& state) {
  if (asks.betsAsk <= state.served.betsAsk) {
    return;
  }
  BetsReading reading{.ask = asks.betsAsk,
                      .round = asks.betsRound,
                      .receipt = {},
                      .refusal = {}};
  reading.refusal = failureOf(
      [&client, &asks, &reading] {
        reading.receipt =
            client.placeBets(asks.token, asks.betsRound, asks.bets);
      },
      betsRefusalOf);
  state.bets.publish(reading);
  state.served.betsAsk = asks.betsAsk;
}

/** @brief Reads the room, the bets of a round and the rooms @p asks asks,
 * each when its ask is newer than the one @p state served. */
void serveReads(network::MasterCasinoClient& client, const CasinoAsks& asks,
                CasinoLinkState& state) {
  if (asks.roomAsk > state.served.roomAsk) {
    state.room.publish(
        CasinoRoomReading{.ask = asks.roomAsk,
                          .room = asks.room,
                          .state = readingOrNothing([&client, &asks] {
                            return client.readRoom(asks.room);
                          })});
    state.served.roomAsk = asks.roomAsk;
  }
  if (asks.ownBetsAsk > state.served.ownBetsAsk) {
    state.ownBets.publish(OwnBetsReading{
        .ask = asks.ownBetsAsk,
        .round = asks.ownBetsRound,
        .bets = readingOrNothing([&client, &asks] {
          return client.readOwnBets(asks.token, asks.ownBetsRound);
        })});
    state.served.ownBetsAsk = asks.ownBetsAsk;
  }
  if (asks.roomsAsk > state.served.roomsAsk) {
    state.rooms.publish(CasinoRoomsReading{
        .ask = asks.roomsAsk,
        .listing = readingOrNothing([&client] { return client.listRooms(); })});
    state.served.roomsAsk = asks.roomsAsk;
  }
}

/** @brief The loop of the thread: waits for a newer ask, and serves it with
 * @p client. */
void runCasinoLink(const std::stop_token& stop,
                   network::MasterCasinoClient& client,
                   CasinoLinkState& state) {
  std::uint64_t seen = 0;
  while (!stop.stop_requested()) {
    seen = state.asks.waitForNewer(seen, CASINO_ASK_WAIT, stop);
    const std::optional<CasinoAsks> asks = state.asks.value();
    if (asks.has_value()) {
      serveSeat(client, *asks, state);
      serveBets(client, *asks, state);
      serveReads(client, *asks, state);
    }
  }
}

}  // namespace

void MasterCasinoLink::start(const std::string& url) {
  auto client = std::make_unique<network::MasterCasinoClient>(url);
  thread_ = std::jthread(
      [this, client = std::move(client)](const std::stop_token& stop) {
        runCasinoLink(stop, *client, state_);
      });
}

void MasterCasinoLink::stop() { thread_ = std::jthread{}; }

std::uint64_t MasterCasinoLink::askRooms() {
  ++asked_.roomsAsk;
  state_.asks.publish(asked_);
  return asked_.roomsAsk;
}

std::uint64_t MasterCasinoLink::askRoom(std::uint8_t room) {
  ++asked_.roomAsk;
  asked_.room = room;
  state_.asks.publish(asked_);
  return asked_.roomAsk;
}

std::uint64_t MasterCasinoLink::askSeat(const std::string& token,
                                        CasinoSeat seat) {
  ++asked_.seatAsk;
  asked_.seat = seat;
  asked_.token = token;
  state_.asks.publish(asked_);
  return asked_.seatAsk;
}

std::uint64_t MasterCasinoLink::askBets(const std::string& token,
                                        network::RoomRound round,
                                        const network::RouletteBets& bets) {
  ++asked_.betsAsk;
  asked_.betsRound = round;
  asked_.bets = bets;
  asked_.token = token;
  state_.asks.publish(asked_);
  return asked_.betsAsk;
}

std::uint64_t MasterCasinoLink::askOwnBets(const std::string& token,
                                           network::RoomRound round) {
  ++asked_.ownBetsAsk;
  asked_.ownBetsRound = round;
  asked_.token = token;
  state_.asks.publish(asked_);
  return asked_.ownBetsAsk;
}

CasinoReadings MasterCasinoLink::readings() const {
  return CasinoReadings{.rooms = state_.rooms.value(),
                        .room = state_.room.value(),
                        .seat = state_.seat.value(),
                        .bets = state_.bets.value(),
                        .ownBets = state_.ownBets.value()};
}

}  // namespace rtype::client
