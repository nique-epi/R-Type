#pragma once

#include <memory>
#include <string>

namespace httplib {
class Client;
struct Request;
}  // namespace httplib

namespace rtype::network {

/**
 * @brief The HTTP connection to the master the clients of the master share:
 * cpp-httplib, kept open between calls as long as the master keeps it, with
 * MASTER_CONNECTION_TIMEOUT to connect and MASTER_TRANSFER_TIMEOUT for each
 * read and write. One call at a time: two threads never send through the same
 * transport together.
 */
class MasterTransport {
 public:
  /**
   * @throws InvalidMasterUrlException when @p url is not
   * `http(s)://<host>[:<port>]`, or its port is outside 1 to 65535. An
   * `https://` master is trusted only with a certificate the roots of the
   * system sign for its host.
   */
  explicit MasterTransport(const std::string& url);
  ~MasterTransport();

  MasterTransport(const MasterTransport&) = delete;
  MasterTransport& operator=(const MasterTransport&) = delete;
  MasterTransport(MasterTransport&&) = delete;
  MasterTransport& operator=(MasterTransport&&) = delete;

  /**
   * @returns The body the master answered to @p request, which carries
   * @p credential as Bearer unless it is empty.
   * @throws MasterUnreachableException when no answer comes.
   * @throws MasterRefusedException with the status and the `detail` of the
   * problem document when the master answers an error status.
   */
  [[nodiscard]] std::string send(httplib::Request request,
                                 const std::string& credential);

 private:
  std::unique_ptr<httplib::Client> client_;
};

}  // namespace rtype::network
