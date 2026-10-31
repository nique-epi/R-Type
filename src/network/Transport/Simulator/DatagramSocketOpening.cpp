#include "DatagramSocketOpening.hpp"
#include <memory>
#include <string>
#include <utility>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "Logger.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConditionsText.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "SimulatedNetworkSocket.hpp"
#include "SimulatorConstants.hpp"
#include "UdpSocket.hpp"

namespace rtype::network {

namespace {

/** @brief Writes, as a warning, what the simulated network does to the
 * datagrams, so a journal shows the simulator was on. */
void announceSimulatedNetwork(const NetworkConditions& conditions) {
  const logging::Logger logger{std::string{NETWORK_LOGGER_NAME}};
  logger.warn("network simulated: ", conditionsText(conditions));
}

}  // namespace

std::unique_ptr<IDatagramSocket> openDatagramSocket(
    NetworkContext& context, const Endpoint& localEndpoint,
    const NetworkConditions& conditions) {
  auto socket = std::make_unique<UdpSocket>(context, localEndpoint);
  if (conditions == PERFECT_NETWORK) {
    return socket;
  }
  announceSimulatedNetwork(conditions);
  return std::make_unique<SimulatedNetworkSocket>(context, std::move(socket),
                                                  conditions);
}

}  // namespace rtype::network
