#include "ServerLobbyModule.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptor.hpp"
#include "Endpoint.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::string ServerLobbyModule::name() const {
  return std::string{LOBBY_MODULE_NAME};
}

std::vector<std::string> ServerLobbyModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}, std::string{GAMES_MODULE_NAME}};
}

void ServerLobbyModule::install(engine::InstallContext& context) {
  games_ = &context.require<Games>();
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  context.addWork(SIMULATION_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    answerConnectionRequests();
                  });
}

void ServerLobbyModule::start() {
  GameInstance& joinable = games_->joinable();
  IDatagramSender* const sender = sender_;
  IPlayerAdmission* const admission = admission_;
  acceptor_ = std::make_unique<ConnectionAcceptor>(
      joinable.roster(),
      [sender](network::OutgoingDatagram datagram) {
        sender->queueDatagram(std::move(datagram));
      },
      [admission](const network::Endpoint& player, std::uint64_t sessionToken) {
        admission->admit(player, sessionToken);
      },
      [&joinable](ConnectedPlayer& player) {
        joinable.onPlayerJoined(player);
      });
}

void ServerLobbyModule::stop() { acceptor_.reset(); }

void ServerLobbyModule::answerConnectionRequests() {
  const GameInstance& joinable = games_->joinable();
  acceptor_->setGameInProgress(joinable.state() !=
                               GameState::WaitingForPlayers);
  acceptor_->receive(tickDatagrams_->connectionRequests(), clock_.now());
}

}  // namespace rtype::server
