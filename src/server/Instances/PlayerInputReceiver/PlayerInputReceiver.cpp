#include "PlayerInputReceiver.hpp"
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "InstancesConstants.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether @p input is newer than @p newest: no input was kept yet, or
 * its number is higher. An input numbered like the one kept is not newer: it
 * is that input again, or one the player did not send.
 */
bool isNewer(const network::PlayerInput& input,
             const std::optional<network::PlayerInput>& newest) {
  return !newest.has_value() || input.inputNumber > newest->inputNumber;
}

/**
 * @returns Whether @p input holds other actions than @p newest, or no input
 * was kept yet.
 */
bool changesActions(const network::PlayerInput& input,
                    const std::optional<network::PlayerInput>& newest) {
  return !newest.has_value() || input.actions != newest->actions;
}

}  // namespace

PlayerInputReceiver::PlayerInputReceiver(PlayerRoster& roster)
    : roster_(&roster), logger_(std::string{INSTANCES_LOGGER_NAME}) {
  dispatcher_.registerHandler<network::PlayerInput>(
      [this](const network::MessageOrigin& origin,
             const network::PlayerInput& input) {
        keepIfNewer(origin, input);
      });
}

void PlayerInputReceiver::receive(
    std::span<const network::IncomingDatagram> datagrams) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    read(datagram);
  }
}

std::size_t PlayerInputReceiver::foreignTokenCount() const {
  return foreignTokenCount_;
}

void PlayerInputReceiver::read(const network::IncomingDatagram& datagram) {
  try {
    const network::PacketHeader header =
        network::DatagramCodec::decodeHeader(datagram.payload);
    if (!network::DatagramCodec::speaksThisProtocol(header)) {
      return;
    }
    const std::vector<network::MessageFrame> messages =
        network::DatagramCodec::splitMessages(datagram.payload);
    const ConnectedPlayer* const player = roster_->find(datagram.sender);
    if (player == nullptr) {
      return;
    }
    if (header.sessionToken != player->sessionToken) {
      dropForeignToken(datagram.sender);
      return;
    }
    dispatcher_.dispatch({.sender = datagram.sender, .header = header},
                         messages);
  } catch (const network::MalformedDatagramException&) {
    return;
  }
}

void PlayerInputReceiver::dropForeignToken(const network::Endpoint& sender) {
  ++foreignTokenCount_;
  logger_.debug("dropped a datagram from ", sender.address, " port ",
                sender.port, ": it carries another session token");
}

void PlayerInputReceiver::keepIfNewer(const network::MessageOrigin& origin,
                                      const network::PlayerInput& input) {
  ConnectedPlayer* const player = roster_->find(origin.sender);
  if (player == nullptr || !isNewer(input, player->newestInput)) {
    return;
  }
  if (changesActions(input, player->newestInput)) {
    logger_.debug("player ", static_cast<unsigned int>(player->playerNumber),
                  " holds actions ", static_cast<unsigned int>(input.actions));
  }
  player->newestInput = input;
}

}  // namespace rtype::server
