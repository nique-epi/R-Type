#pragma once

#include <span>
#include <string_view>
#include "ChatEntry.hpp"

namespace rtype::client {

/** @brief The chat of the session: what the player sends, and the lines the
 * server relays. Screens read it at each update. */
class IChatChannel {
 public:
  virtual ~IChatChannel() = default;

  /**
   * @brief Sends @p text to the server once the lines before it got their
   * answer.
   * @returns Whether the line was taken: never without a session, nor for a
   * text isValidChatText() refuses, nor while CHAT_LINES_WAITING lines wait,
   * when a line saying so joins the chat.
   */
  virtual bool send(std::string_view text) = 0;

  /** @returns The lines kept, oldest first, CHAT_LINES_KEPT at most; valid
   * until the next frame. */
  [[nodiscard]] virtual std::span<const ChatEntry> entries() const = 0;
};

}  // namespace rtype::client
