#pragma once

#include <any>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <utility>
#include "Logger.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief Hands each received message to the handler registered for its type,
 * so the code that receives datagrams never lists the messages: adding a
 * message is a struct with its payload functions, and one registerHandler()
 * call where it is handled.
 *
 * A message whose type has no handler is skipped, and so is a message whose
 * payload its reader refuses; each is counted and logged at the debug level,
 * and the next message is handled. No exception caused by the content of a
 * message leaves dispatch(), so one forged message cannot stop the program. An
 * exception thrown by a handler does leave it: it is a fault of the program,
 * not of the datagram.
 *
 * Each message is handled on its own, and a handler cannot tell which
 * messages shared a datagram. A sender the server does not know yet must be
 * answered at most once per datagram, or a forged address would make the
 * server send more bytes than it received: the code that receives must
 * enforce it before dispatching.
 *
 * Not thread-safe: register the handlers, then dispatch, on one thread.
 */
class MessageDispatcher {
 public:
  template <typename Message>
  using Handler = std::function<void(const MessageOrigin&, const Message&)>;

  MessageDispatcher();

  /**
   * @brief Calls @p handler with every later message of type Message. The
   * type is written explicitly: registerHandler<ReadyState>(...).
   * @throws EmptyMessageHandlerException when @p handler is empty.
   * @throws DuplicateMessageHandlerException when Message already has a
   * handler: a type has one handler. Whatever it throws, the handlers already
   * registered are unchanged.
   */
  template <DecodableMessage Message>
  void registerHandler(Handler<Message> handler) {
    if (!handler) {
      throw EmptyMessageHandlerException();
    }
    Route& slot = routes_.at(static_cast<std::uint8_t>(Message::TYPE));
    if (slot.decode) {
      throw DuplicateMessageHandlerException(
          static_cast<std::uint8_t>(Message::TYPE));
    }
    Route route{.decode =
                    [](std::span<const std::byte> payload) {
                      return std::any{decodeMessage<Message>(payload)};
                    },
                .handle =
                    [typedHandler = std::move(handler)](
                        const MessageOrigin& origin, const std::any& message) {
                      typedHandler(origin,
                                   std::any_cast<const Message&>(message));
                    }};
    slot = std::move(route);
  }

  /**
   * @brief Hands each of @p messages, in order, to the handler of its type,
   * with @p origin. The payloads are read before their handler is called; a
   * message that cannot be read never reaches it.
   * @throws What a handler throws; the messages after it are not handled.
   */
  void dispatch(const MessageOrigin& origin,
                std::span<const MessageFrame> messages);

  /**
   * @returns How many messages were skipped because no handler was
   * registered for their type.
   */
  [[nodiscard]] std::size_t skippedMessageCount() const;

  /**
   * @returns How many messages were dropped because their payload is not one
   * the protocol allows for their type.
   */
  [[nodiscard]] std::size_t malformedMessageCount() const;

 private:
  /**
   * @brief What a type's handler is made of: reading its payload into a
   * message, then calling the handler with it. Both are empty for a type
   * without a handler.
   */
  struct Route {
    std::function<std::any(std::span<const std::byte>)> decode;
    std::function<void(const MessageOrigin&, const std::any&)> handle;
  };

  void dispatchOne(const MessageOrigin& origin, const MessageFrame& message);
  void skipUnhandled(const MessageOrigin& origin, std::uint8_t type);
  void dropMalformed(const MessageOrigin& origin, std::uint8_t type,
                     const NetworkException& error);

  std::array<Route, MESSAGE_TYPE_COUNT> routes_;
  std::size_t skippedMessageCount_{0};
  std::size_t malformedMessageCount_{0};
  logging::Logger logger_;
};

}  // namespace rtype::network
