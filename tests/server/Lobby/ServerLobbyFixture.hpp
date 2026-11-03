#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "ServerGamesFixture.hpp"

/** @brief The base of the tests of the lobby module: the games fixture with
 * the lobby module listed after the games module. */
class ServerLobbyFixture : public ServerGamesFixture {
 protected:
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>>
  modulesAfterTheGames() const override;

  /** @returns A connection request from the player at @p index that carries
   * @p cookie, none when it is 0. */
  [[nodiscard]] static rtype::network::IncomingDatagram requestFrom(
      std::size_t index, std::uint64_t cookie = 0);

  /** @returns The cookie of the challenge the server last sent to the player
   * at @p index. */
  [[nodiscard]] std::uint64_t challengeCookieOf(std::size_t index);

  /** @brief Runs one tick in which the player at @p index, already seated,
   * says it is ready. */
  void sayReady(std::size_t index);
};
