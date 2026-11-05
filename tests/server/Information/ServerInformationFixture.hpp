#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "IModule.hpp"
#include "ServerInformation.hpp"
#include "ServerLobbyFixture.hpp"

/** @brief The base of the tests of the information module: the lobby fixture
 * with the information module listed after the lobby module. */
class ServerInformationFixture : public ServerLobbyFixture {
 protected:
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>>
  modulesAfterTheGames() const override;

  /** @brief Runs one tick in which the player at @p index asks for the
   * information of the server.
   * @returns The answer it got; the test fails when it got none. */
  [[nodiscard]] rtype::network::ServerInformation askFrom(std::size_t index);
};
