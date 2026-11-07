#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ChatChannelConstants.hpp"
#include "ChatEntry.hpp"
#include "ChatLine.hpp"
#include "IChatChannel.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "PlayerChatLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The chat of the session with the server. A line sent goes out at
 * once, then again every CHAT_LINE_INTERVAL until the server relays it back
 * with the number of this player, for CHAT_GIVE_UP_TIME at most; the lines
 * sent meanwhile wait, CHAT_LINES_WAITING at most, since the protocol takes
 * one line at a time. Every line the server relays joins the chat with the
 * nickname the waiting room gives its writer; the client keeps
 * CHAT_LINES_KEPT, and forgets them all when the session closes.
 */
class ChatChannel final : public IChatChannel, public ISessionObserver {
 public:
  /** @param dispatcher Where the handler of PlayerChatLine is registered; it
   * must outlive this object.
   * @param room The waiting room of the session, which names the writers; it
   * must outlive this object. */
  ChatChannel(network::MessageDispatcher& dispatcher, const IWaitingRoom& room);

  bool send(std::string_view text) override;
  [[nodiscard]] std::span<const ChatEntry> entries() const override;

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;
  void sessionClosed() override;

  /** @brief Ages the lines, and sends the line in flight again or gives up
   * on it, as time passes. */
  void sessionHeld(engine::Duration elapsed) override;

 private:
  void receive(const network::PlayerChatLine& relayed);
  /** @brief Sends the line waiting first, if any. */
  void sendNextLine();
  void followLineInFlight(engine::Duration elapsed);
  /** @brief Adds a line to the chat, dropping the oldest past
   * CHAT_LINES_KEPT. */
  void keep(ChatEntry entry);
  [[nodiscard]] std::string nicknameOf(std::uint8_t playerNumber) const;

  const IWaitingRoom* room_;
  ISessionOutbox* outbox_{nullptr};
  std::uint8_t ownNumber_{};
  std::uint16_t nextLineNumber_{FIRST_CHAT_LINE_NUMBER};
  std::deque<std::string> waiting_;
  std::optional<network::ChatLine> inFlight_;
  engine::Duration sinceSent_{};
  engine::Duration sinceFirstSent_{};
  std::vector<ChatEntry> entries_;
};

}  // namespace rtype::client
