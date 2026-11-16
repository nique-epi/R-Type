#pragma once

#include <httplib.h>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "CannedAnswer.hpp"
#include "RecordedRequest.hpp"

/**
 * @brief An HTTP server on the loopback that stands in for the routes of the
 * accounts of the master, on a port the system chooses: it answers each route
 * with what the test gave it, 404 for any other, and records every request,
 * so a test reads the token, the path and the body a client sent.
 *
 * The real master answers these routes over HTTPS only; a client reaches
 * this one over plain HTTP, which tests the client without TLS.
 */
class FakeAccountMaster {
 public:
  FakeAccountMaster();
  ~FakeAccountMaster();
  FakeAccountMaster(const FakeAccountMaster&) = delete;
  FakeAccountMaster& operator=(const FakeAccountMaster&) = delete;
  FakeAccountMaster(FakeAccountMaster&&) = delete;
  FakeAccountMaster& operator=(FakeAccountMaster&&) = delete;

  /** @brief Answers every later request to @p route, its method and its path
   * (`GET /api/users/me`), with @p answer. */
  void answer(const std::string& route, CannedAnswer answer);

  /** @returns Where a client reaches it: `http://127.0.0.1:<port>`. */
  [[nodiscard]] std::string url() const;

  /** @returns Every request it received, oldest first. */
  [[nodiscard]] std::vector<RecordedRequest> requests() const;

 private:
  void serve(const httplib::Request& request, httplib::Response& response);

  mutable std::mutex mutex_;
  std::map<std::string, CannedAnswer> answers_;
  std::vector<RecordedRequest> requests_;
  httplib::Server server_;
  int port_{};
  std::thread listener_;
};
