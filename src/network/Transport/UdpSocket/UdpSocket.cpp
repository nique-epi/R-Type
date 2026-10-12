#include "UdpSocket.hpp"
#include <array>
#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/udp.hpp>
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "Logger.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkException.hpp"

namespace rtype::network {

namespace {

/**
 * @throws InvalidAddressException when the address of @p endpoint is not an
 *         IP address.
 */
asio::ip::udp::endpoint toAsioEndpoint(const Endpoint& endpoint) {
  std::error_code error;
  const asio::ip::address address =
      asio::ip::make_address(endpoint.address, error);
  if (error) {
    throw InvalidAddressException(endpoint.address);
  }
  return {address, endpoint.port};
}

Endpoint toEndpoint(const asio::ip::udp::endpoint& endpoint) {
  return Endpoint{.address = endpoint.address().to_string(),
                  .port = endpoint.port()};
}

/**
 * @return whether @p error only reports that an earlier datagram reached a
 *         port nobody listens on. Windows reports it on a later receive, and
 *         Asio turns it into connection_refused (complete_iocp_recvfrom in
 *         asio/detail/impl/socket_ops.ipp).
 */
bool isPortUnreachableReport(const std::error_code& error) {
  return error == asio::error::connection_refused;
}

}  // namespace

/**
 * @brief The Asio state of a UdpSocket, kept alive by every operation in
 *        flight.
 *
 * The next receive is started only after the handler returned: Asio may fill
 * the buffer as soon as a receive starts, while the handler still reads it.
 */
struct UdpSocket::Implementation
    : std::enable_shared_from_this<UdpSocket::Implementation> {
  explicit Implementation(asio::io_context& ioContext);

  void receiveNext();
  void handleReceived(const std::error_code& error, std::size_t byteCount);

  asio::ip::udp::socket socket;
  std::array<std::byte, RECEIVE_BUFFER_SIZE> receiveBuffer{};
  asio::ip::udp::endpoint sender;
  DatagramHandler handler;
  bool receiving{false};
  logging::Logger logger{std::string{NETWORK_LOGGER_NAME}};
};

UdpSocket::Implementation::Implementation(asio::io_context& ioContext)
    : socket(ioContext) {}

void UdpSocket::Implementation::receiveNext() {
  socket.async_receive_from(
      asio::buffer(receiveBuffer), sender,
      [self = shared_from_this()](const std::error_code& error,
                                  std::size_t byteCount) {
        self->handleReceived(error, byteCount);
      });
}

void UdpSocket::Implementation::handleReceived(const std::error_code& error,
                                               std::size_t byteCount) {
  if (error == asio::error::operation_aborted || !socket.is_open()) {
    return;
  }
  if (isPortUnreachableReport(error)) {
    logger.debug("an earlier datagram reached a closed port");
  } else if (error) {
    logger.warn("receiving failed, still listening: ", error.message());
  } else if (byteCount > MAX_DATAGRAM_SIZE) {
    logger.debug("dropped a datagram larger than ", MAX_DATAGRAM_SIZE,
                 " bytes from ", sender.address().to_string(), " port ",
                 sender.port());
  } else {
    handler(toEndpoint(sender),
            std::span<const std::byte>{receiveBuffer}.first(byteCount));
  }
  receiveNext();
}

UdpSocket::UdpSocket(NetworkContext& context, const Endpoint& localEndpoint)
    : implementation_(std::make_shared<Implementation>(context.ioContext())) {
  const asio::ip::udp::endpoint asioEndpoint = toAsioEndpoint(localEndpoint);
  std::error_code error;
  implementation_->socket.open(asioEndpoint.protocol(), error);
  if (!error) {
    implementation_->socket.bind(asioEndpoint, error);
  }
  if (error) {
    throw SocketOpenException(localEndpoint.address, localEndpoint.port,
                              error.message());
  }
}

UdpSocket::~UdpSocket() {
  std::error_code ignored;
  implementation_->socket.close(ignored);
}

void UdpSocket::startReceiving(DatagramHandler handler) {
  if (implementation_->receiving) {
    throw ReceivingAlreadyStartedException();
  }
  implementation_->receiving = true;
  implementation_->handler = std::move(handler);
  implementation_->receiveNext();
}

void UdpSocket::send(const Endpoint& destination,
                     std::span<const std::byte> payload) {
  if (payload.size() > MAX_DATAGRAM_SIZE) {
    throw DatagramTooLargeException(payload.size());
  }
  const asio::ip::udp::endpoint asioDestination = toAsioEndpoint(destination);
  auto bytes = std::make_shared<const std::vector<std::byte>>(payload.begin(),
                                                              payload.end());
  implementation_->socket.async_send_to(
      asio::buffer(*bytes), asioDestination,
      [implementation = implementation_, bytes](
          const std::error_code& error,
          [[maybe_unused]] std::size_t byteCount) {
        if (error && error != asio::error::operation_aborted) {
          implementation->logger.warn("sending failed: ", error.message());
        }
      });
}

Endpoint UdpSocket::localEndpoint() const {
  return toEndpoint(implementation_->socket.local_endpoint());
}

}  // namespace rtype::network
