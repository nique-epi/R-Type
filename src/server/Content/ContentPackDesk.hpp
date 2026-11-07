#pragma once

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "ContentFingerprint.hpp"
#include "IClock.hpp"
#include "RequestAllowances.hpp"

namespace rtype::server {

/**
 * @brief Answers network::CONTENT_PACK_ROUTE with the one pack the supervisor
 * plays with, on the threads of the HTTP server.
 *
 * A request naming its fingerprint gets the pack, its ETag the fingerprint; a
 * request naming any other gets 404, so the desk never reads a path to find a
 * file. Each address spends PACK_REQUEST_RATE_LIMIT, the port of the
 * connection left out since each connection takes another: past it, a request
 * gets 429 and nothing is sent. The pack is encoded once, so what one request
 * costs never depends on what the client sends.
 */
class ContentPackDesk {
 public:
  /**
   * @param pack The pack, as network::encodeContentPack() wrote it.
   * @param clock The clock the allowances are counted on; it outlives the
   * desk.
   */
  ContentPackDesk(std::vector<std::byte> pack, const engine::IClock& clock);

  /** @returns The answer to @p request, as the class says. */
  [[nodiscard]] network::ApiResponse answer(const network::ApiRequest& request);

  /** @returns The fingerprint of the pack. */
  [[nodiscard]] const network::ContentFingerprint& fingerprint() const;

  /** @returns The bytes of the pack. */
  [[nodiscard]] std::size_t size() const;

 private:
  /** @returns Whether @p address may ask for the pack now. */
  [[nodiscard]] bool allows(const std::string& address);

  std::string pack_;
  network::ContentFingerprint fingerprint_;
  std::string fingerprintText_;
  const engine::IClock* clock_;
  std::mutex allowancesMutex_;
  network::RequestAllowances allowances_;
};

}  // namespace rtype::server
