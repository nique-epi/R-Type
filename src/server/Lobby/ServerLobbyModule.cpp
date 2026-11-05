#include "ServerLobbyModule.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptor.hpp"
#include "ConnectionAcceptorParts.hpp"
#include "Endpoint.hpp"
#include "EntitySheets.hpp"
#include "EntityTypeEntry.hpp"
#include "EntityTypeTable.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The table of the entity types @p types holds, or an empty one
 * when the games module kept none, which only happens before it added any
 * game, so no acceptor reads it. */
std::vector<network::EntityTypeEntry> entityTypeTableOrNone(
    const game::EntitySheets* types) {
  if (types == nullptr) {
    return {};
  }
  return entityTypeTableOf(*types);
}

}  // namespace

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
  IPlayerAdmission* const admission = admission_;
  const std::vector<network::EntityTypeEntry> entityTypes =
      entityTypeTableOrNone(games_->entityTypes());
  for (const std::unique_ptr<GameInstance>& game : games_->all()) {
    GameInstance* const seating = game.get();
    acceptors_.push_back(
        std::make_unique<ConnectionAcceptor>(ConnectionAcceptorParts{
            .roster = seating->roster(),
            .send = std::bind_front(&IDatagramSender::queueDatagram, sender_),
            .admit =
                [admission](const network::Endpoint& player,
                            std::uint64_t sessionToken) {
                  admission->admit(player, sessionToken);
                },
            .onPlayerJoined =
                [seating](ConnectedPlayer& player) {
                  seating->onPlayerJoined(player);
                },
            .entityTypes = entityTypes}));
  }
}

void ServerLobbyModule::stop() { acceptors_.clear(); }

void ServerLobbyModule::answerConnectionRequests() {
  const engine::Duration now = clock_.now();
  for (const network::IncomingDatagram& request :
       tickDatagrams_->connectionRequests()) {
    const std::size_t game = games_->placeOf(request.sender);
    ConnectionAcceptor& acceptor = *acceptors_.at(game);
    acceptor.setGameInProgress(games_->all()[game]->state() !=
                               GameState::WaitingForPlayers);
    acceptor.receive(std::span{&request, 1}, now);
  }
}

}  // namespace rtype::server
