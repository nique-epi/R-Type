#include "ServerLobbyModule.hpp"
#include <cstdint>
#include <functional>
#include <memory>
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
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "RunningGame.hpp"
#include "ServerConstants.hpp"
#include "ShipGun.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The table of the entity types @p types holds, or an empty one
 * when the games module kept none, which only happens before it created the
 * game, so no acceptor reads it. */
std::vector<network::EntityTypeEntry> entityTypeTableOrNone(
    const game::EntitySheets* types) {
  if (types == nullptr) {
    return {};
  }
  return entityTypeTableOf(*types);
}

/** @returns The gun of the ships of @p types, or one that predicts nothing
 * when the games module kept no types (see entityTypeTableOrNone()). */
network::ShipGun shipGunOrNone(const game::EntitySheets* types) {
  if (types == nullptr) {
    return network::ShipGun{};
  }
  return shipGunOf(*types);
}

}  // namespace

std::string ServerLobbyModule::name() const {
  return std::string{LOBBY_MODULE_NAME};
}

std::vector<std::string> ServerLobbyModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}, std::string{GAMES_MODULE_NAME}};
}

void ServerLobbyModule::install(engine::InstallContext& context) {
  runningGame_ = &context.require<RunningGame>();
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
  GameInstance* const seating = &runningGame_->game();
  acceptor_ = std::make_unique<ConnectionAcceptor>(ConnectionAcceptorParts{
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
      .entityTypes = entityTypeTableOrNone(runningGame_->entityTypes()),
      .shipGun = shipGunOrNone(runningGame_->entityTypes())});
}

void ServerLobbyModule::stop() { acceptor_.reset(); }

void ServerLobbyModule::answerConnectionRequests() {
  acceptor_->setGameInProgress(runningGame_->game().state() !=
                               GameState::WaitingForPlayers);
  acceptor_->receive(tickDatagrams_->connectionRequests(), clock_.now());
}

}  // namespace rtype::server
