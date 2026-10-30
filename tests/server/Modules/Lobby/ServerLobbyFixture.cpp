#include "ServerLobbyFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "ConnectionChallenge.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyModule.hpp"

using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::sentTo;

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerLobbyFixture::modulesAfterTheGames() const {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<rtype::server::ServerLobbyModule>());
  return modules;
}

rtype::network::IncomingDatagram ServerLobbyFixture::requestFrom(
    std::size_t index, std::uint64_t cookie) {
  return rtype::network::IncomingDatagram{
      .sender = playerEndpoint(index),
      .payload = cookie == 0 ? requestDatagram() : requestDatagram(cookie)};
}

void ServerLobbyFixture::sayReady(std::size_t index) {
  rtype::network::PacketHeader header;
  header.sessionToken = tokenOf(index);
  runTick({rtype::network::IncomingDatagram{.sender = playerEndpoint(index),
                                            .payload = readyDatagram(header)}});
}

std::uint64_t ServerLobbyFixture::challengeCookieOf(std::size_t index) {
  const auto received =
      sentTo(network().sender().sent(), playerEndpoint(index));
  return onlyMessageIn<rtype::network::ConnectionChallenge>(received.back())
      .cookie;
}
