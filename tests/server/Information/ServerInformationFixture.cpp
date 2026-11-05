#include "ServerInformationFixture.hpp"
#include <cstddef>
#include <memory>
#include <vector>
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationModule.hpp"
#include "ServerLobbyModule.hpp"

using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::sentTo;

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerInformationFixture::modulesAfterTheGames() const {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<rtype::server::ServerLobbyModule>());
  modules.push_back(std::make_unique<rtype::server::ServerInformationModule>());
  return modules;
}

rtype::network::ServerInformation ServerInformationFixture::askFrom(
    std::size_t index) {
  runTick({}, {},
          {rtype::network::IncomingDatagram{
              .sender = playerEndpoint(index),
              .payload = informationRequestDatagram()}});
  const auto received =
      sentTo(network().sender().sent(), playerEndpoint(index));
  EXPECT_FALSE(received.empty());
  return received.empty() ? rtype::network::ServerInformation{}
                          : onlyMessageIn<rtype::network::ServerInformation>(
                                received.back());
}
