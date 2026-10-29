#include "DatagramReader.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <ratio>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "PlayerDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SessionConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @brief A count of simulation ticks, as a duration: converted to nanoseconds,
 * a whole number of seconds of ticks gives exactly that many seconds.
 */
using SimulationTicks = std::chrono::duration<
    std::uint64_t, std::ratio<1, static_cast<std::intmax_t>(
                                     engine::SIMULATION_TICKS_PER_SECOND)>>;

}  // namespace

DatagramReader::DatagramReader(PlayerRoster& roster)
    : roster_(roster),
      rejectionLog_(std::string{SESSIONS_LOGGER_NAME},
                    network::REJECTION_LINE_INTERVAL) {}

std::optional<PlayerDatagram> DatagramReader::read(
    const network::IncomingDatagram& datagram) {
  network::PacketHeader header;
  std::vector<network::MessageFrame> messages;
  try {
    header = network::DatagramCodec::decodeHeader(datagram.payload);
    if (!network::DatagramCodec::speaksThisProtocol(header)) {
      drop(datagram.sender, network::FOREIGN_HEADER_DROP_REASON);
      return std::nullopt;
    }
    messages = network::DatagramCodec::splitMessages(datagram.payload);
  } catch (const network::NetworkException& error) {
    drop(datagram.sender, error.what());
    return std::nullopt;
  }
  ConnectedPlayer* const player = roster_.find(datagram.sender);
  if (player == nullptr) {
    drop(datagram.sender, NOT_A_PLAYER_DROP_REASON);
    return std::nullopt;
  }
  if (!player->arrivals.accept(header.sequence)) {
    drop(datagram.sender, NOT_NEWER_DROP_REASON);
    return std::nullopt;
  }
  return PlayerDatagram{
      .player = player,
      .origin =
          network::MessageOrigin{.sender = datagram.sender, .header = header},
      .messages = std::move(messages)};
}

void DatagramReader::countTick() { ++ticksCounted_; }

void DatagramReader::drop(const network::Endpoint& sender,
                          std::string_view reason) {
  rejectionLog_.debug(timeRun(), "dropped a datagram from ", sender.address,
                      " port ", sender.port, ": ", reason);
}

std::chrono::nanoseconds DatagramReader::timeRun() const {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
      SimulationTicks{ticksCounted_});
}

}  // namespace rtype::server
