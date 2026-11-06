#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "MapDirection.hpp"
#include "MapView.hpp"
#include "ServerMapStage.hpp"
#include "ServerRow.hpp"
#include "StarBearing.hpp"

namespace rtype::client {

/**
 * @brief The servers on the map, the one chosen, and how deep the player went:
 * what the keys of the map change. A new list keeps the chosen server when it
 * is still listed, by its id, and chooses the first one otherwise, the lowest
 * ping; a list of no server goes back to the whole map.
 */
class ServerMapChoice {
 public:
  void showServers(std::vector<ServerRow> servers);
  [[nodiscard]] const std::vector<ServerRow>& servers() const;
  [[nodiscard]] std::optional<ServerRow> chosen() const;
  [[nodiscard]] ServerMapStage stage() const;

  /** @brief Goes one stage deeper, from the whole map to the games; nothing
   * without a server. */
  void deeper();
  /** @returns Whether it went one stage back; false on the whole map. */
  bool shallower();

  /** @brief Chooses the star an arrow towards @p direction reaches. */
  void moveTowards(MapDirection direction);
  /** @brief Chooses the next server by ping, or the previous one when @p step
   * is negative, going round. */
  void moveByPing(int step);

  /**
   * @brief Chooses the star under @p pointer, a pixel of the grid, as
   * starUnder() finds it, the games of a server left open.
   * @returns Whether a star was under it; nothing changes otherwise.
   */
  bool chooseAt(sf::Vector2i pointer);

  /** @returns How the map is seen at this stage: the whole map, or zoomed on
   * the chosen server. */
  [[nodiscard]] MapView view() const;

 private:
  std::vector<ServerRow> servers_;
  std::size_t chosen_{};
  ServerMapStage stage_{ServerMapStage::ZoomedOut};
};

/** @returns Where @p server stands from the planet: its ping, or the farthest
 * ring for a server of another version, which gives none, and the direction
 * its id takes. */
[[nodiscard]] StarBearing bearingOf(const ServerRow& server);

/** @returns The pixel each server of @p choice stands on in its view(), in
 * the order of its servers(). */
[[nodiscard]] std::vector<sf::Vector2i> starPointsOf(
    const ServerMapChoice& choice);

}  // namespace rtype::client
