#include "LobbyModule.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptor.hpp"
#include "Endpoint.hpp"
#include "GameInstance.hpp"
#include "GameList.hpp"
#include "GameState.hpp"
#include "IServerLink.hpp"
#include "InstallContext.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::string LobbyModule::name() const { return std::string{LOBBY_MODULE_NAME}; }

std::vector<std::string> LobbyModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}, std::string{GAMES_MODULE_NAME}};
}

void LobbyModule::install(engine::InstallContext& context) {
  link_ = &context.require<IServerLink>();
  received_ = &context.require<const ServerApplication::ReceivedDatagrams>();
  games_ = &context.require<GameList>();
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { answerRequests(); });
}

void LobbyModule::start() {
  GameInstance& joinable = games_->joinable();
  IServerLink& link = *link_;
  acceptor_ = std::make_unique<ConnectionAcceptor>(
      joinable.roster(),
      [&link](network::OutgoingDatagram datagram) {
        link.send(std::move(datagram));
      },
      [&link](const network::Endpoint& player, std::uint64_t sessionToken) {
        link.admit(player, sessionToken);
      },
      [&joinable](ConnectedPlayer& player) {
        joinable.onPlayerJoined(player);
      });
}

void LobbyModule::stop() { acceptor_.reset(); }

void LobbyModule::answerRequests() {
  acceptor_->setGameInProgress(games_->joinable().state() !=
                               GameState::WaitingForPlayers);
  acceptor_->receive(received_->connectionRequests, clock_.now());
}

}  // namespace rtype::server
