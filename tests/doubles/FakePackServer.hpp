#pragma once

#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

/**
 * @brief An HTTP server on the loopback that stands in for a supervisor
 * serving its content pack at one path, on a port the system chooses.
 *
 * It answers like the real one, a range included, and can be told to cut its
 * first answer after some bytes, closing the connection as a network that
 * drops it would, or to answer every request with another status.
 */
class FakePackServer {
 public:
  FakePackServer(std::string path, std::string body);
  ~FakePackServer();
  FakePackServer(const FakePackServer&) = delete;
  FakePackServer& operator=(const FakePackServer&) = delete;
  FakePackServer(FakePackServer&&) = delete;
  FakePackServer& operator=(FakePackServer&&) = delete;

  /** @brief Cuts the first answer after @p bytes; call it before a request. */
  void cutTheFirstAnswerAfter(std::size_t bytes);

  /** @brief Answers every request with @p status and no body. */
  void answerEveryRequestWith(int status);

  [[nodiscard]] std::uint16_t port() const;

  /** @returns How many requests reached the path. */
  [[nodiscard]] std::size_t requests() const;

  /** @returns The Range header of the last request, if it had one. */
  [[nodiscard]] std::optional<std::string> lastRange() const;

 private:
  void answer(const httplib::Request& request, httplib::Response& response);

  std::string body_;
  mutable std::mutex mutex_;
  std::optional<std::size_t> cutAfter_;
  std::optional<int> status_;
  std::size_t requests_{};
  std::optional<std::string> lastRange_;
  httplib::Server server_;
  int port_{};
  std::thread listener_;
};
