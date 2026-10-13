#pragma once

#include <cstdint>
#include <functional>
#include <span>
#include "ComponentRegistry.hpp"
#include "Endpoint.hpp"
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

  /**
   * @returns The number of ticks counted, which is the tick of the last state
   * sent or due.
   */
  [[nodiscard]] std::uint32_t currentTick() const;

 private:
  [[nodiscard]] network::WorldState capture(
      engine::ComponentRegistry& components) const;

  Send send_;
  std::uint32_t tick_{0};
};

}  // namespace rtype::server
