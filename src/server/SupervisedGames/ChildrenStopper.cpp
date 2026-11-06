#include "ChildrenStopper.hpp"
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "ExitedGame.hpp"
#include "GameRegistry.hpp"
#include "IDatagramSender.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "StopPause.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @brief Adds @p more to the end of @p exited. */
void append(std::vector<ExitedGame>& exited, std::vector<ExitedGame> more) {
  exited.insert(exited.end(), std::make_move_iterator(more.begin()),
                std::make_move_iterator(more.end()));
}

}  // namespace

ChildrenStopper::ChildrenStopper(IDatagramSender& sender, StopPause pause)
    : sender_(&sender), pause_(std::move(pause)) {}

std::vector<ExitedGame> ChildrenStopper::stopAll(GameRegistry& registry) {
  std::vector<ExitedGame> exited;
  for (std::size_t copy = 0; copy < FAREWELL_COPIES; ++copy) {
    queueStopOrders(registry);
    pause_(FAREWELL_COPY_INTERVAL);
    append(exited, registry.collectExits());
  }
  engine::Duration waited = engine::Duration::zero();
  while (!registry.all().empty() && waited < STOP_GRACE_WAIT) {
    pause_(STOP_POLL_INTERVAL);
    waited += STOP_POLL_INTERVAL;
    append(exited, registry.collectExits());
  }
  registry.terminateAll();
  append(exited, registry.collectExits());
  return exited;
}

void ChildrenStopper::queueStopOrders(const GameRegistry& registry) {
  for (const SupervisedGame& game : registry.all()) {
    sender_->queueDatagram(network::OutgoingDatagram{
        .destination =
            network::Endpoint{
                .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
                .port = game.port},
        .payload = network::DatagramCodec::encode(
            network::PacketHeader{.sessionToken = game.linkToken},
            network::Disconnection{})});
  }
}

}  // namespace rtype::server
