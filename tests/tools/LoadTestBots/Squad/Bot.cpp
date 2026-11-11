#include "Bot.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string_view>
#include <utility>
#include "AssetFolder.hpp"
#include "BotReport.hpp"
#include "BotSquadConstants.hpp"
#include "BotStanding.hpp"
#include "BotTraffic.hpp"
#include "ContentPlaces.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "GameRequest.hpp"
#include "IBotPilot.hpp"
#include "NetworkConditionsConstants.hpp"
#include "NetworkThread.hpp"
#include "TimeConstants.hpp"
#include "TrafficTotals.hpp"

namespace rtype::tools {

namespace {

/** @returns What crossed the socket of @p thread since it was built, as the
 * client reads it and as the network carried it. */
BotTraffic trafficOf(client::NetworkThread& thread) {
  return BotTraffic{
      .plain = network::trafficTotalsOf(thread.traffic().report()),
      .wire = network::trafficTotalsOf(thread.wireTraffic().report())};
}

/** @returns Where a bot finds its own content, the assets folder next to it,
 * and where it keeps the packs of servers: BOT_PACK_CACHE_FOLDER of the
 * temporary folder of the system. A bot draws nothing, so its content has
 * no appearance. */
client::ContentPlaces botContentPlaces() {
  return client::ContentPlaces{
      .assetFolder = engine::locateAssetFolder(),
      .cacheFolder =
          std::filesystem::temp_directory_path() / BOT_PACK_CACHE_FOLDER};
}

}  // namespace

Bot::Bot(std::unique_ptr<IBotPilot> pilot)
    : connection_(network::PERFECT_NETWORK, botContentPlaces()),
      room_(&connection_.waitingRoom()),
      sight_(connection_.entityTypes()),
      inputs_(std::move(pilot), connection_.waitingRoom(),
              connection_.session()) {}

void Bot::join(const network::Endpoint& server, std::string_view nickname) {
  connection_.network().open();
  connection_.session().connect(server, nickname, client::GameRequest{});
}

void Bot::update(engine::Duration elapsed) {
  connection_.update(elapsed);
  readReceived(elapsed);
  if (inputs_.advance(elapsed, sight_)) {
    tally_.countInput();
  }
}

void Bot::askReady() { connection_.waitingRoom().setReady(true); }

BotStanding Bot::standing() const {
  return BotStanding{.phase = connection_.session().status().phase,
                     .gameStarted = room_->gameStarted(),
                     .requestedReady = room_->requestedReady()};
}

BotReport Bot::report() const {
  BotReport report = tally_.report();
  report.phase = connection_.session().status().phase;
  report.scoreEarned = sight_.figures().scoreEarned;
  report.bossFights = boss_.fights();
  return report;
}

const BotSight& Bot::sight() const { return sight_; }

void Bot::leave() {
  connection_.leave();
  connection_.network().close();
}

void Bot::readReceived(engine::Duration elapsed) {
  received_.clear();
  connection_.entityMessages().drainEntityMessages(received_);
  const std::uint8_t playerNumber = connection_.session().status().playerNumber;
  for (const client::EntityMessage& message : received_) {
    sight_.read(message, playerNumber);
    tally_.count(message);
    boss_.count(message);
    inputs_.confirm(message, playerNumber);
  }
  boss_.addTime(elapsed, sight_.figures());
  const bool gameStarted = connection_.waitingRoom().gameStarted();
  tally_.addTime(elapsed, gameStarted);
  tally_.addTraffic(trafficOf(connection_.network()), gameStarted);
}

}  // namespace rtype::tools
