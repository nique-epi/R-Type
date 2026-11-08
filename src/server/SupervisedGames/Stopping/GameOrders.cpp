#include "GameOrders.hpp"
#include "Disconnection.hpp"
#include "OutgoingDatagram.hpp"
#include "SupervisedGame.hpp"

namespace rtype::server {

network::OutgoingDatagram stopOrderTo(const SupervisedGame& game) {
  return orderTo(game, network::Disconnection{});
}

}  // namespace rtype::server
