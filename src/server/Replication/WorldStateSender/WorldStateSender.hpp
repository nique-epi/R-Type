#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include "ComponentRegistry.hpp"
#include "Endpoint.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "WorldState.hpp"

namespace rtype::server {

/**
 * @brief Counts the simulation ticks and, every TICKS_PER_WORLD_STATE of them,
 * sends the state of the world to every recipient.
 *
 * The state lists every entity that has a NetworkIdentity, a Position and an
 * EntityType, each under its network id. It is sent as several datagrams, with
 * the same tick, when the entities do not fit one. Nothing is built when there
 * is no recipient.
 *
 * An entity whose position is not a finite number, which only a game bug can
 * produce, is left out of the state instead of stopping the server; a warning
 * gives how many entities were left out, each time that number changes.
 */
class WorldStateSender {
 public:
  /**
   * @brief What the sender calls for each datagram to send.
   */
  using Send = std::function<void(network::OutgoingDatagram)>;

  /**
   * @param send Must not be empty.
   */
  explicit WorldStateSender(Send send);

  /**
   * @brief Counts one more tick and, when it is one that carries a state,
   * sends the state to @p recipients. Call it once per tick, on the
   * simulation thread.
   */
  void onTick(engine::ComponentRegistry& components,
              std::span<const network::Endpoint> recipients);

 private:
  [[nodiscard]] network::WorldState capture(
      engine::ComponentRegistry& components, std::size_t& leftOutCount) const;
  void reportLeftOut(std::size_t leftOutCount);

  Send send_;
  std::uint32_t tick_{0};
  std::size_t reportedLeftOutCount_{0};
  logging::Logger logger_;
};

}  // namespace rtype::server
