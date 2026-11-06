#include "ServerMapChoice.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "MapDirection.hpp"
#include "MapView.hpp"
#include "SectorLayout.hpp"
#include "SectorMapConstants.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerMapStage.hpp"
#include "ServerRow.hpp"
#include "StarBearing.hpp"
#include "StarNavigation.hpp"

namespace rtype::client {

StarBearing bearingOf(const ServerRow& server) {
  const double ping =
      server.ping.has_value()
          ? std::chrono::duration<double, std::milli>(*server.ping).count()
          : PING_OF_A_SERVER_OF_ANOTHER_VERSION;
  return StarBearing{.pingMilliseconds = ping,
                     .angleDegrees = angleOf(server.id, ping)};
}

void ServerMapChoice::showServers(std::vector<ServerRow> servers) {
  const std::optional<ServerRow> before = chosen();
  servers_ = std::move(servers);
  chosen_ = 0;
  if (before.has_value()) {
    const auto still = std::ranges::find(servers_, before->id, &ServerRow::id);
    if (still != servers_.end()) {
      chosen_ = static_cast<std::size_t>(still - servers_.begin());
    }
  }
  if (servers_.empty()) {
    stage_ = ServerMapStage::ZoomedOut;
  }
}

const std::vector<ServerRow>& ServerMapChoice::servers() const {
  return servers_;
}

std::optional<ServerRow> ServerMapChoice::chosen() const {
  if (chosen_ >= servers_.size()) {
    return std::nullopt;
  }
  return servers_[chosen_];
}

ServerMapStage ServerMapChoice::stage() const { return stage_; }

void ServerMapChoice::deeper() {
  if (servers_.empty()) {
    return;
  }
  stage_ = stage_ == ServerMapStage::ZoomedOut ? ServerMapStage::ZoomedIn
                                               : ServerMapStage::Games;
}

bool ServerMapChoice::shallower() {
  switch (stage_) {
    case ServerMapStage::Games:
      stage_ = ServerMapStage::ZoomedIn;
      return true;
    case ServerMapStage::ZoomedIn:
      stage_ = ServerMapStage::ZoomedOut;
      return true;
    case ServerMapStage::ZoomedOut:
      break;
  }
  return false;
}

void ServerMapChoice::moveTowards(MapDirection direction) {
  const std::vector<sf::Vector2i> stars = points();
  if (const std::optional<std::size_t> next =
          neighbourOf(stars, chosen_, direction)) {
    chosen_ = *next;
  }
}

void ServerMapChoice::moveByPing(int step) {
  if (servers_.empty()) {
    return;
  }
  const auto count = static_cast<long>(servers_.size());
  const long moved = (static_cast<long>(chosen_) + step) % count;
  chosen_ = static_cast<std::size_t>(moved < 0 ? moved + count : moved);
}

MapView ServerMapChoice::view() const {
  const std::optional<ServerRow> server = chosen();
  if (stage_ == ServerMapStage::ZoomedOut || !server.has_value()) {
    return MapView{.zoom = ZOOMED_OUT_FACTOR,
                   .centerX = SECTOR_PLANET_CENTER.x,
                   .centerY = SECTOR_PLANET_CENTER.y};
  }
  return viewOn(bearingOf(*server), ZOOMED_IN_FACTOR, CHOSEN_STAR_ZOOMED_IN);
}

std::vector<sf::Vector2i> ServerMapChoice::points() const {
  const MapView seen = view();
  std::vector<sf::Vector2i> stars;
  stars.reserve(servers_.size());
  for (const ServerRow& server : servers_) {
    stars.push_back(placeOf(bearingOf(server), seen));
  }
  return stars;
}

}  // namespace rtype::client
