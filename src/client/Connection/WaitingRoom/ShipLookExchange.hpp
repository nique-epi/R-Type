#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "IShipLookExchange.hpp"
#include "IWaitingRoom.hpp"
#include "MessageDispatcher.hpp"
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Sends the look the player chose to the server and keeps the looks the
 * server shows.
 *
 * chooseLook() sends a ShipLookChoice at once, then again every
 * SHIP_LOOK_CHOICE_INTERVAL until a PlayerLooks lists the look on the player:
 * a lost message is made up by the next, and the server answers a copy.
 * Nothing is sent once the game started. The items won are kept across the
 * close of a session, so an item won just before a disconnection still reaches
 * the locker. Not thread-safe: use it from the thread that updates the
 * session.
 */
class ShipLookExchange final : public IShipLookExchange,
                               public ISessionObserver {
 public:
  /**
   * @param dispatcher Where the handlers of the messages are registered; it
   * must outlive the exchange.
   * @param room Tells whether the game started; it must outlive the exchange.
   */
  ShipLookExchange(network::MessageDispatcher& dispatcher,
                   const IWaitingRoom& room);

  void chooseLook(identifiers::ShipLook look) override;
  [[nodiscard]] std::span<const network::PlayerLook> playerLooks()
      const override;
  [[nodiscard]] std::vector<std::uint8_t> takeWonItems() override;

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;
  void sessionClosed() override;
  void sessionHeld(engine::Duration elapsed) override;

 private:
  void sendChoice(identifiers::ShipLook look);

  const IWaitingRoom* room_;
  ISessionOutbox* outbox_{nullptr};
  std::uint8_t ownPlayerNumber_{0};
  std::optional<identifiers::ShipLook> requestedLook_;
  std::vector<network::PlayerLook> looks_;
  std::vector<std::uint8_t> wonItems_;
  engine::Duration sinceChoice_{};
};

}  // namespace rtype::client
