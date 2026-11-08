#include "ServerSupervisorLobbyModule.hpp"
#include <optional>
#include <string>
#include <vector>
#include "BanBook.hpp"
#include "BanCandidate.hpp"
#include "BanList.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "CurrentSecond.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "GamePlace.hpp"
#include "GameRegistry.hpp"
#include "GameRules.hpp"
#include "GameSpawner.hpp"
#include "HostedDifficulties.hpp"
#include "IDatagramSender.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "MessagePayload.hpp"
#include "ModuleConstants.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlaceKind.hpp"
#include "ProtocolConstants.hpp"
#include "RequestReading.hpp"
#include "RulesRefusal.hpp"
#include "SeatRequest.hpp"
#include "ServerConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns A datagram to @p destination that holds @p message, under a
 * header without session, as every answer to a sender not in a session. */
template <network::EncodableMessage Message>
network::OutgoingDatagram datagramTo(const network::Endpoint& destination,
                                     const Message& message) {
  return network::OutgoingDatagram{.destination = destination,
                                   .payload = network::DatagramCodec::encode(
                                       network::PacketHeader{}, message)};
}

/** @returns Who opens the game a request of @p game starts: its sender
 * for a NewGame, nobody otherwise. */
std::optional<network::Endpoint> creatorOf(const SeatRequest& request,
                                           const network::GameChoice& game) {
  if (game.kind == network::GameChoiceKind::NewGame) {
    return request.sender;
  }
  return std::nullopt;
}

/** @returns The refusal of @p message, when it opens a game with rules
 * @p difficulties or the bounds of the lives refuse (see refusalOf());
 * nothing otherwise, a request of any other choice included. */
std::optional<GamePlace> refusedRulesOf(
    const network::ConnectionRequest& message,
    const HostedDifficulties& difficulties) {
  if (!message.rules.has_value()) {
    return std::nullopt;
  }
  const std::optional<network::RefusalReason> refusal =
      refusalOf(*message.rules, difficulties);
  if (!refusal.has_value()) {
    return std::nullopt;
  }
  return GamePlace{.kind = PlaceKind::Refuse, .refusal = *refusal};
}

/** @returns Whether a ban of @p bans bars the nickname or the address of
 * @p request now. */
bool isBarred(const BanList& bans, const SeatRequest& request) {
  return bans.bars(BanCandidate{.nickname = request.nickname,
                                .address = request.sender.address,
                                .at = currentSecond()});
}

}  // namespace

std::string ServerSupervisorLobbyModule::name() const {
  return std::string{SUPERVISOR_LOBBY_MODULE_NAME};
}

std::vector<std::string> ServerSupervisorLobbyModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME},
          std::string{SUPERVISED_GAMES_MODULE_NAME},
          std::string{BANS_MODULE_NAME}};
}

void ServerSupervisorLobbyModule::install(engine::InstallContext& context) {
  registry_ = &context.require<GameRegistry>();
  difficulties_ = &context.require<HostedDifficulties>();
  spawner_ = &context.require<GameSpawner>();
  sender_ = &context.require<IDatagramSender>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  bans_ = &context.require<BanBook>();
  context.addWork(SIMULATION_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    answerConnectionRequests();
                  });
}

void ServerSupervisorLobbyModule::start() {}

void ServerSupervisorLobbyModule::stop() {}

void ServerSupervisorLobbyModule::answerConnectionRequests() {
  const engine::Duration now = clock_.now();
  for (const network::IncomingDatagram& datagram :
       tickDatagrams_->connectionRequests()) {
    answer(datagram, now);
  }
}

void ServerSupervisorLobbyModule::answer(
    const network::IncomingDatagram& datagram, engine::Duration now) {
  if (isOfAnotherVersion(datagram.payload)) {
    refuseAnotherVersion(datagram);
    return;
  }
  std::optional<network::ConnectionRequest> request;
  try {
    request = firstConnectionRequest(datagram.payload);
  } catch (const network::NetworkException&) {
    return;
  }
  if (!request.has_value()) {
    return;
  }
  if (!cookies_.isValid(request->cookie, datagram.sender, now)) {
    sender_->queueDatagram(datagramTo(
        datagram.sender, network::ConnectionChallenge{
                             .cookie = cookies_.issue(datagram.sender, now)}));
    return;
  }
  const SeatRequest seat{.sender = datagram.sender,
                         .nickname = request->nickname};
  if (isBarred(bans_->list(), seat)) {
    sender_->queueDatagram(datagramTo(
        datagram.sender,
        network::ConnectionRefused{.reason = network::RefusalReason::Banned}));
    return;
  }
  answerValidRequest(seat, *request);
}

void ServerSupervisorLobbyModule::refuseAnotherVersion(
    const network::IncomingDatagram& datagram) {
  if (datagram.payload.size() < network::CONNECTION_REFUSED_DATAGRAM_SIZE) {
    return;
  }
  sender_->queueDatagram(
      datagramTo(datagram.sender,
                 network::ConnectionRefused{
                     .reason = network::RefusalReason::IncompatibleVersion}));
}

void ServerSupervisorLobbyModule::answerValidRequest(
    const SeatRequest& request, const network::ConnectionRequest& message) {
  const network::GameChoice& game = message.game;
  const std::optional<GamePlace> refused =
      refusedRulesOf(message, *difficulties_);
  const GamePlace place =
      refused.has_value() ? *refused : registry_->placeFor(request, game);
  switch (place.kind) {
    case PlaceKind::Redirect:
      sender_->queueDatagram(datagramTo(
          request.sender, network::ConnectionRedirect{.port = place.port}));
      break;
    case PlaceKind::Wait:
      break;
    case PlaceKind::Open:
      if (spawner_->openGame(
              creatorOf(request, game),
              message.rules.value_or(network::defaultGameRules()))) {
        static_cast<void>(registry_->placeFor(request, game));
      }
      break;
    case PlaceKind::Refuse:
      sender_->queueDatagram(datagramTo(
          request.sender, network::ConnectionRefused{.reason = place.refusal}));
      break;
  }
}

}  // namespace rtype::server
