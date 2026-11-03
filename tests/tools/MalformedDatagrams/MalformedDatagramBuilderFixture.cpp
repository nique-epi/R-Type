#include "MalformedDatagramBuilderFixture.hpp"
#include <cstddef>
#include <span>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "GameStarted.hpp"
#include "Heartbeat.hpp"
#include "LobbyState.hpp"
#include "MalformedDatagramBuilderTestConstants.hpp"
#include "MalformedDatagramKind.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "ReadyState.hpp"
#include "WorldState.hpp"

namespace rtype::tools::testing {

namespace {

template <network::DecodableMessage Message>
void readEvery(network::MessageDispatcher& dispatcher) {
  dispatcher.registerHandler<Message>(
      [](const network::MessageOrigin&, const Message&) {});
}

}  // namespace

MalformedDatagramBuilderFixture::MalformedDatagramBuilderFixture()
    : builder_(SOME_SEED) {
  readEvery<network::ConnectionRequest>(dispatcher_);
  readEvery<network::ConnectionAccepted>(dispatcher_);
  readEvery<network::ConnectionRefused>(dispatcher_);
  readEvery<network::Disconnection>(dispatcher_);
  readEvery<network::Heartbeat>(dispatcher_);
  readEvery<network::ReadyState>(dispatcher_);
  readEvery<network::LobbyState>(dispatcher_);
  readEvery<network::GameStarted>(dispatcher_);
  readEvery<network::PlayerInput>(dispatcher_);
  readEvery<network::WorldState>(dispatcher_);
  readEvery<network::EntitySpawned>(dispatcher_);
  readEvery<network::EntityDestroyed>(dispatcher_);
  readEvery<network::GameEvent>(dispatcher_);
  readEvery<network::PlayerLeft>(dispatcher_);
  readEvery<network::GameEnded>(dispatcher_);
  readEvery<network::ConnectionChallenge>(dispatcher_);
}

std::vector<std::vector<std::byte>> MalformedDatagramBuilderFixture::draw(
    MalformedDatagramKind kind) {
  std::vector<std::vector<std::byte>> datagrams;
  datagrams.reserve(DRAWS_PER_KIND);
  for (std::size_t index = 0; index < DRAWS_PER_KIND; ++index) {
    datagrams.push_back(builder_.build(kind, SOME_HEADER));
  }
  return datagrams;
}

void MalformedDatagramBuilderFixture::dispatch(
    std::span<const std::byte> datagram) {
  dispatcher_.dispatch(
      network::MessageOrigin{
          .sender = network::Endpoint{},
          .header = network::DatagramCodec::decodeHeader(datagram)},
      network::DatagramCodec::splitMessages(datagram));
}

std::size_t MalformedDatagramBuilderFixture::refusedCount() const {
  return dispatcher_.malformedMessageCount();
}

std::size_t MalformedDatagramBuilderFixture::skippedCount() const {
  return dispatcher_.skippedMessageCount();
}

}  // namespace rtype::tools::testing
