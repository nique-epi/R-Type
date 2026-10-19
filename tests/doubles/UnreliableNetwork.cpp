#include "UnreliableNetwork.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <random>
#include <utility>
#include <vector>
#include "IncomingDatagram.hpp"

void UnreliableNetwork::send(rtype::network::IncomingDatagram datagram) {
  sent_.push_back(std::move(datagram));
}

std::vector<rtype::network::IncomingDatagram> UnreliableNetwork::arrivals(
    std::initializer_list<std::size_t> sendIndexes) const {
  std::vector<rtype::network::IncomingDatagram> arrived;
  for (const std::size_t sendIndex : sendIndexes) {
    arrived.push_back(sent_.at(sendIndex));
  }
  return arrived;
}

std::vector<rtype::network::IncomingDatagram>
UnreliableNetwork::shuffledArrivals(std::uint32_t seed) const {
  std::mt19937 generator{seed};
  std::bernoulli_distribution duplicated;
  std::vector<rtype::network::IncomingDatagram> arrived;
  for (const rtype::network::IncomingDatagram& datagram : sent_) {
    arrived.push_back(datagram);
    if (duplicated(generator)) {
      arrived.push_back(datagram);
    }
  }
  std::ranges::shuffle(arrived, generator);
  return arrived;
}
