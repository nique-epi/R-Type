#pragma once

#include <vector>
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"

namespace rtype::client {

/**
 * @brief A datagram link that reads the datagrams of one sender: the share of
 * a socket that one recipient owns.
 */
class IRoutedLink : public IDatagramLink {
 public:
  /**
   * @brief From now on, reads the datagrams of @p sender and no others. The
   * datagrams that arrived before this call are dropped, whoever sent them: a
   * recipient never reads a late answer of an earlier exchange.
   */
  virtual void listenTo(const network::Endpoint& sender) = 0;

  /**
   * @brief From now on, reads the datagrams of every sender of @p senders and
   * no others, as listenTo() does for one: the datagrams that arrived before
   * this call are dropped. An empty list listens to nobody.
   */
  virtual void listenToEach(std::vector<network::Endpoint> senders) = 0;
};

}  // namespace rtype::client
