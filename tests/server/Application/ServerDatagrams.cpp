#include "ServerDatagrams.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "PacketHeader.hpp"
#include "ReadyState.hpp"
#include "ServerTestConstants.hpp"

std::vector<std::byte> requestDatagram(
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(
      header, rtype::network::ConnectionRequest{
                  .cookie = 0, .nickname = std::string{REQUEST_NICKNAME}});
}

std::vector<std::byte> readyDatagram(
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(
      header, rtype::network::ReadyState{.ready = true});
}
