#include "CasinoService.hpp"
#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <utility>
#include <variant>
#include <vector>
#include "BetOrder.hpp"
#include "BetRefusal.hpp"
#include "BetsReceipt.hpp"
#include "CasinoBetRepository.hpp"
#include "CasinoPlayer.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomConstants.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomNames.hpp"
#include "CasinoRoomState.hpp"
#include "CasinoRoundRepository.hpp"
#include "CasinoSeatRepository.hpp"
#include "CasinoSettings.hpp"
#include "ChipMovement.hpp"
#include "ChipMovementResult.hpp"
#include "IAccountWallet.hpp"
#include "MasterDatabase.hpp"
#include "MillisecondDate.hpp"
#include "MovementReason.hpp"
#include "PlayerBet.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteConstants.hpp"
#include "RouletteDurations.hpp"
#include "RoulettePhase.hpp"
#include "RouletteRules.hpp"
#include "RouletteSchedule.hpp"
#include "RoundBets.hpp"
#include "RoundPayouts.hpp"
#include "RoundReference.hpp"
#include "ScheduledRound.hpp"
#include "SeatKey.hpp"
#include "SeatOutcome.hpp"
#include "SeatedPlayer.hpp"

namespace rtype::master {

namespace {

std::chrono::sys_seconds secondOf(network::MillisecondDate date) {
  return std::chrono::floor<std::chrono::seconds>(date);
}

/** @returns The oldest a seat may have been seen at @p now and still
 * count. */
std::chrono::sys_seconds seatsSeenSince(std::chrono::sys_seconds now) {
  return now - network::CASINO_SEAT_LIFETIME;
}

/** @returns Why the rules of the cloth refuse @p bets, nothing when they
 * accept them. */
std::optional<BetRefusal> ruleRefusalOf(
    const std::vector<network::RouletteBet>& bets) {
  if (bets.size() > network::MAXIMUM_ROULETTE_BETS) {
    return BetRefusal::TooManyBets;
  }
  for (const network::RouletteBet& bet : bets) {
    if (bet.amount == 0 || !network::isPlaceableBet(bet)) {
      return BetRefusal::InvalidBet;
    }
  }
  if (!network::fitsRoundCaps(bets)) {
    return BetRefusal::OverTheCap;
  }
  return std::nullopt;
}

/** @returns Why the table refuses @p order at its date: its round is not the
 * one whose bets are open, or was drawn; its account is not seated. */
std::optional<BetRefusal> tableRefusalOf(SQLite::Database& connection,
                                         const BetOrder& order,
                                         const RouletteDurations& durations) {
  const ScheduledRound scheduled = scheduledRoundAt(order.sentAt, durations);
  if (order.place.round != scheduled.round ||
      scheduled.phase != network::RoulettePhase::Betting ||
      CasinoRoundRepository{connection}.numberOf(order.place).has_value()) {
    return BetRefusal::BetsClosed;
  }
  const std::optional<std::chrono::sys_seconds> seen =
      CasinoSeatRepository{connection}.seenAt(
          SeatKey{.room = order.place.room, .userId = order.userId});
  if (!seen.has_value() || *seen < seatsSeenSince(secondOf(order.sentAt))) {
    return BetRefusal::NotSeated;
  }
  return std::nullopt;
}

/** @returns The one movement that turns a stake of @p previous chips on the
 * round of @p order into the stake of its bets: a stake for the chips it adds,
 * a refund for the chips it hands back, and nothing to write when both are
 * equal. */
ChipMovement stakeChangeOf(const BetOrder& order, std::uint64_t previous) {
  const auto change = static_cast<std::int64_t>(previous) -
                      static_cast<std::int64_t>(network::stakeOf(order.bets));
  return ChipMovement{.userId = order.userId,
                      .amount = change,
                      .reason = change > 0 ? network::MovementReason::Refund
                                           : network::MovementReason::Stake,
                      .reference = roundReferenceOf(order.place),
                      .madeAt = secondOf(order.sentAt)};
}

/** @brief What the table shows of the players of a round: the stake and the
 * bets of each account, and its payout once the round was drawn. */
struct RoundTable {
  std::map<std::int64_t, std::uint32_t> stakes;
  std::map<std::int64_t, std::vector<network::RouletteBet>> bets;
  std::optional<std::map<std::int64_t, std::uint32_t>> payouts;
};

/** @returns What the table shows of @p seated: its stake and its bets in
 * @p table, and its payout once the round was drawn. */
network::CasinoPlayer playerOf(const SeatedPlayer& seated,
                               const RoundTable& table) {
  network::CasinoPlayer player{.nickname = seated.nickname,
                               .stake = 0,
                               .payout = std::nullopt,
                               .bets = {}};
  if (const auto stake = table.stakes.find(seated.userId);
      stake != table.stakes.end()) {
    player.stake = stake->second;
  }
  if (const auto bets = table.bets.find(seated.userId);
      bets != table.bets.end()) {
    player.bets = bets->second;
  }
  if (table.payouts.has_value()) {
    const auto payout = table.payouts->find(seated.userId);
    player.payout = payout == table.payouts->end() ? 0 : payout->second;
  }
  return player;
}

/** @brief Fills the number, the history and the players of @p state, whose
 * room, round and phase are set, as @p connection holds them at @p now. */
void fillRoomState(SQLite::Database& connection,
                   network::CasinoRoomState& state,
                   std::chrono::sys_seconds now) {
  const network::RoomRound place{.room = state.id, .round = state.round};
  CasinoRoundRepository rounds{connection};
  if (state.phase != network::RoulettePhase::Betting) {
    state.winningNumber = rounds.numberOf(place);
  }
  state.history = rounds.numbersBefore(place, network::CASINO_HISTORY_LENGTH);
  const std::vector<PlayerBet> bets =
      CasinoBetRepository{connection}.roundBetsOf(place);
  RoundTable table{.stakes = stakesOf(bets),
                   .bets = betsByPlayerOf(bets),
                   .payouts = std::nullopt};
  if (state.winningNumber.has_value()) {
    table.payouts = payoutsOf(bets, *state.winningNumber);
  }
  for (const SeatedPlayer& seated :
       CasinoSeatRepository{connection}.seatedSince(state.id,
                                                    seatsSeenSince(now))) {
    state.players.push_back(playerOf(seated, table));
  }
}

}  // namespace

CasinoService::CasinoService(MasterDatabase& database, IAccountWallet& wallet,
                             std::function<CasinoSettings()> settings)
    : database_(database), wallet_(wallet), settings_(std::move(settings)) {}

bool CasinoService::hasRoom(std::uint8_t room) const {
  return room >= network::FIRST_CASINO_ROOM &&
         room - network::FIRST_CASINO_ROOM < settings_().rooms;
}

SeatOutcome CasinoService::takeSeat(SeatKey seat,
                                    std::chrono::sys_seconds now) {
  const std::int64_t seats = settings_().seats;
  return database_.transact([seat, now, seats](SQLite::Database& connection) {
    CasinoSeatRepository repository{connection};
    const std::chrono::sys_seconds since = seatsSeenSince(now);
    const std::optional<std::chrono::sys_seconds> seen =
        repository.seenAt(seat);
    const bool seated = seen.has_value() && *seen >= since;
    if (!seated && repository.othersSeatedSince(seat, since) >= seats) {
      return SeatOutcome::RoomFull;
    }
    repository.markSeen(seat, now);
    return seated ? SeatOutcome::Renewed : SeatOutcome::Taken;
  });
}

void CasinoService::leaveSeat(SeatKey seat) {
  database_.transact([seat](SQLite::Database& connection) {
    CasinoSeatRepository{connection}.remove(seat);
  });
}

std::variant<network::BetsReceipt, BetRefusal> CasinoService::placeBets(
    const BetOrder& order) {
  if (const std::optional<BetRefusal> refusal = ruleRefusalOf(order.bets)) {
    return *refusal;
  }
  const RouletteDurations durations = settings_().durations;
  return database_.transact(
      [this, &order, &durations](SQLite::Database& connection)
          -> std::variant<network::BetsReceipt, BetRefusal> {
        if (const std::optional<BetRefusal> refusal =
                tableRefusalOf(connection, order, durations)) {
          return *refusal;
        }
        return writtenBets(connection, order);
      });
}

network::RoundBets CasinoService::betsOf(network::RoomRound place,
                                         std::int64_t userId) {
  return database_.run([place, userId](SQLite::Database& connection) {
    network::RoundBets answer{
        .round = place.round,
        .bets = CasinoBetRepository{connection}.betsOf(place, userId),
        .stake = 0,
        .payout = std::nullopt,
        .winningNumber = CasinoRoundRepository{connection}.numberOf(place)};
    answer.stake = static_cast<std::uint32_t>(network::stakeOf(answer.bets));
    if (answer.winningNumber.has_value()) {
      answer.payout = static_cast<std::uint32_t>(
          network::payoutOfBets(answer.bets, *answer.winningNumber));
    }
    return answer;
  });
}

network::CasinoRoomListing CasinoService::rooms(network::MillisecondDate now) {
  const CasinoSettings settings = settings_();
  const ScheduledRound scheduled = scheduledRoundAt(now, settings.durations);
  const std::map<std::uint8_t, std::int64_t> counts =
      database_.run([now](SQLite::Database& connection) {
        return CasinoSeatRepository{connection}.countsSince(
            seatsSeenSince(secondOf(now)));
      });
  network::CasinoRoomListing listing;
  for (std::uint8_t offset = 0; offset < settings.rooms; ++offset) {
    const auto room =
        static_cast<std::uint8_t>(network::FIRST_CASINO_ROOM + offset);
    const auto seated = counts.find(room);
    listing.rooms.push_back(
        network::CasinoRoom{.id = room,
                            .name = casinoRoomName(room),
                            .seats = settings.seats,
                            .seated = static_cast<std::uint8_t>(
                                seated == counts.end() ? 0 : seated->second),
                            .round = scheduled.round,
                            .phase = scheduled.phase});
  }
  return listing;
}

network::CasinoRoomState CasinoService::roomState(
    std::uint8_t room, network::MillisecondDate now) {
  const CasinoSettings settings = settings_();
  const ScheduledRound scheduled = scheduledRoundAt(now, settings.durations);
  network::CasinoRoomState state{.id = room,
                                 .name = casinoRoomName(room),
                                 .seats = settings.seats,
                                 .round = scheduled.round,
                                 .phase = scheduled.phase,
                                 .closesAt = scheduled.closesAt,
                                 .resultAt = scheduled.resultAt,
                                 .endsAt = scheduled.endsAt,
                                 .serverTime = now,
                                 .winningNumber = std::nullopt,
                                 .history = {},
                                 .players = {}};
  database_.run([&state, now](SQLite::Database& connection) {
    fillRoomState(connection, state, secondOf(now));
  });
  return state;
}

std::variant<network::BetsReceipt, BetRefusal> CasinoService::writtenBets(
    SQLite::Database& connection, const BetOrder& order) {
  CasinoBetRepository bets{connection};
  const auto stake = static_cast<std::uint32_t>(network::stakeOf(order.bets));
  const ChipMovementResult moved = wallet_.applyChips(
      connection, stakeChangeOf(order, network::stakeOf(bets.betsOf(
                                           order.place, order.userId))));
  if (moved.refusal.has_value()) {
    return BetRefusal::NotEnoughChips;
  }
  bets.removeOf(order.place, order.userId);
  bets.insert(order.place, order.userId, order.bets);
  return network::BetsReceipt{.round = order.place.round,
                              .bets = order.bets,
                              .stake = stake,
                              .chips = moved.chips};
}

}  // namespace rtype::master
