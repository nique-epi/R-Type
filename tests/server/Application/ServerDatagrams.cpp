#include "ServerDatagrams.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ReadyState.hpp"
#include "ServerLobbyTestConstants.hpp"

std::vector<std::byte> requestDatagram(
    const rtype::network::PacketHeader& header) {
  return requestDatagram(0, header);
}

std::vector<std::byte> requestDatagram(
    std::uint64_t cookie, const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(
      header,
      rtype::network::ConnectionRequest{
          .cookie = cookie,
          .nickname = std::string{rtype::server::testing::PLAYER_NICKNAME}});
}

std::vector<std::byte> readyDatagram(
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(
      header, rtype::network::ReadyState{.ready = true});
}

std::vector<std::byte> inputDatagram(
    const rtype::network::PlayerInput& input,
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(header, input);
}
