#include "ServerSupervisorLinkFixture.hpp"
#include <chrono>
#include <cstddef>
#include <future>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IModule.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingNetworkModule.hpp"
#include "ServerSupervisorLinkModule.hpp"
#include "SupervisorLink.hpp"
#include "SupervisorLinkTestConstants.hpp"
#include "TimeConstants.hpp"

ServerSupervisorLinkFixture::~ServerSupervisorLinkFixture() {
  stopRequest_.request();
}

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerSupervisorLinkFixture::modulesAfterTheGames() const {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<rtype::server::ServerSupervisorLinkModule>(
      rtype::server::SupervisorLink{.port = SUPERVISOR_PORT,
                                    .token = LINK_TOKEN},
      stopRequest_));
  return modules;
}

rtype::network::Endpoint ServerSupervisorLinkFixture::supervisor() {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = SUPERVISOR_PORT};
}

void ServerSupervisorLinkFixture::runFor(rtype::engine::Duration duration) {
  for (rtype::engine::Duration done = rtype::engine::Duration::zero();
       done < duration; done += rtype::engine::SIMULATION_TICK_DURATION) {
    runTick({});
  }
}

bool ServerSupervisorLinkFixture::stopWasRequested() {
  std::future<void> waiting = std::async(
      std::launch::async, [this] { static_cast<void>(stopRequest_.wait()); });
  const bool requested =
      waiting.wait_for(STOP_OBSERVATION_WAIT) == std::future_status::ready;
  if (!requested) {
    waiting_.push_back(std::move(waiting));
  }
  return requested;
}

std::vector<rtype::network::OutgoingDatagram>
ServerSupervisorLinkFixture::statuses() {
  std::vector<rtype::network::OutgoingDatagram> found;
  for (const rtype::network::OutgoingDatagram& datagram :
       network().sender().sent()) {
    const auto types =
        rtype::network::DatagramCodec::messageTypes(datagram.payload);
    if (datagram.destination == supervisor() && types.has_value() &&
        types->test(static_cast<std::size_t>(
            rtype::network::MessageType::GameStatus))) {
      found.push_back(datagram);
    }
  }
  return found;
}
