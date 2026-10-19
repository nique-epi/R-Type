#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include "IncomingDatagram.hpp"

/**
 * @brief Stand-in for the network between the clients and the server: the
 * datagrams a test sends arrive in the order the test gives, or in an order
 * drawn from a seed, so a datagram can arrive late, twice or never, as UDP
 * allows.
 */
class UnreliableNetwork {
 public:
  /** @brief Sends @p datagram; it arrives only in a delivery that names it. */
  void send(rtype::network::IncomingDatagram datagram);

  /**
   * @returns The datagrams sent, in the order of @p sendIndexes, where 0 is
   * the first datagram sent. An index may come twice, a duplicate, or never,
   * a loss.
   */
  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> arrivals(
      std::initializer_list<std::size_t> sendIndexes) const;

  /**
   * @returns Every datagram sent, each once or twice, in an order drawn from
   * @p seed. The same seed gives the same arrivals with the same standard
   * library.
   */
  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> shuffledArrivals(
      std::uint32_t seed) const;

 private:
  std::vector<rtype::network::IncomingDatagram> sent_;
};
