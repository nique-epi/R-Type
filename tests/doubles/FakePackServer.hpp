#pragma once

#include <httplib.h>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include "ServedPack.hpp"

/**
 * @brief An HTTP server on the loopback that stands in for a supervisor
 * serving its content pack at one path, on a port the system chooses.
 *
 * It answers like the real one, a range included, and can be told to cut its
 * first answer after some bytes, closing the connection as a network that
 * drops it would, to answer every request with another status, or to hold
 * its answers until a test lets them go, so a download stays in progress for
 * as long as the test needs.
 */
class FakePackServer {
 public:
  explicit FakePackServer(ServedPack pack);
  ~FakePackServer();
  FakePackServer(const FakePackServer&) = delete;
  FakePackServer& operator=(const FakePackServer&) = delete;
  FakePackServer(FakePackServer&&) = delete;
  FakePackServer& operator=(FakePackServer&&) = delete;

  /** @brief Cuts the first answer after @p bytes; call it before a request. */
  void cutTheFirstAnswerAfter(std::size_t bytes);

  /** @brief Answers every request with @p status and no body. */
  void answerEveryRequestWith(int status);

  /** @brief Holds every answer until releaseTheAnswers(). */
  void holdTheAnswers();

  /** @brief Lets every answer held, and every later one, go. */
  void releaseTheAnswers();

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
  bool held_{false};
  std::condition_variable released_;
  httplib::Server server_;
  int port_{};
  std::thread listener_;
};
