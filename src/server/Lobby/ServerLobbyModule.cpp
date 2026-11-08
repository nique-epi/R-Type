#include "ServerLobbyModule.hpp"
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "AssetFolder.hpp"
#include "BanCandidate.hpp"
#include "BanList.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptor.hpp"
#include "ConnectionAcceptorParts.hpp"
#include "ContentAnnouncementReading.hpp"
#include "CurrentSecond.hpp"
#include "Endpoint.hpp"
#include "Expulsion.hpp"
#include "ExpulsionRule.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "LatestBans.hpp"
#include "ModuleConstants.hpp"
#include "RunningGame.hpp"
#include "ServerConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

ServerLobbyModule::ServerLobbyModule(std::uint16_t contentPort)
    : contentPort_(contentPort) {}

std::string ServerLobbyModule::name() const {
  return std::string{LOBBY_MODULE_NAME};
}

std::vector<std::string> ServerLobbyModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}, std::string{GAMES_MODULE_NAME}};
}

std::vector<std::string> ServerLobbyModule::optionalDependencies() const {
  return {std::string{STORAGE_MODULE_NAME}};
}

void ServerLobbyModule::install(engine::InstallContext& context) {
  runningGame_ = &context.require<RunningGame>();
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  bans_ = context.find<LatestBans>();
  content_ = contentAnnouncementOf(engine::locateAssetFolder(), contentPort_);
  context.addWork(SIMULATION_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    answerConnectionRequests();
                    expelBannedPilots();
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
      .content = content_,
      .bans = bans_});
}

void ServerLobbyModule::stop() { acceptor_.reset(); }

void ServerLobbyModule::answerConnectionRequests() {
  acceptor_->setGameInProgress(runningGame_->game().state() !=
                               GameState::WaitingForPlayers);
  acceptor_->receive(tickDatagrams_->connectionRequests(), clock_.now());
}

void ServerLobbyModule::expelBannedPilots() {
  if (bans_ == nullptr || bans_->revision() == seenBans_) {
    return;
  }
  seenBans_ = bans_->revision();
  const BanList bans = bans_->value().value_or(BanList{});
  const std::chrono::sys_seconds now = currentSecond();
  expeller_.expel(
      std::bind_front(&IDatagramSender::queueDatagram, sender_),
      runningGame_->game().roster(),
      ExpulsionRule{.chosen =
                        [&bans, now](const ConnectedPlayer& player) {
                          return bans.bars(
                              BanCandidate{.nickname = player.nickname,
                                           .address = player.endpoint.address,
                                           .at = now});
                        },
                    .reason = network::ExpulsionReason::Banned});
}

}  // namespace rtype::server
