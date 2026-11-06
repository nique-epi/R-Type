#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include "AttemptProgress.hpp"
#include "ConnectionRequest.hpp"
#include "ISessionOutbox.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The request of the client to join a server, as the Connection
 * section of the protocol page describes: a ConnectionRequest without a
 * cookie, sent again every network::CONNECTION_REQUEST_INTERVAL while the
 * server does not answer, and with the cookie of its ConnectionChallenge once
 * it sent one. An attempt that got neither an acceptance nor a refusal within
 * CONNECTION_ATTEMPT_TIMEOUT of begin() is timed out: a challenge does not
 * restart that delay.
 */
class ConnectionAttempt {
 public:
  /** @brief Starts a new attempt as @p nickname, asking for @p game in every
   * request, the ones after a redirect included: no cookie yet, no time
   * elapsed. Sends nothing. */
  void begin(std::string_view nickname, const network::GameChoice& game);

  /**
   * @brief Sends the request, with the cookie if there is one, and starts the
   * delay before the next one over.
   * @throws InvalidNicknameException when the nickname is not 1 to
   * MAXIMUM_NICKNAME_SIZE bytes of UTF-8; nothing is sent then.
   */
  void sendRequest(ISessionOutbox& outbox);

  /** @brief Keeps @p cookie and sends the request carrying it at once. */
  void answerChallenge(std::uint64_t cookie, ISessionOutbox& outbox);

  /**
   * @returns Whether a redirect may be followed now: the first one of the
   * attempt may, a second one may not, so a redirect never makes another.
   * Asking uses it up.
   */
  [[nodiscard]] bool acceptRedirect();

  /** @brief Forgets the cookie, which belonged to the server that redirected
   * the attempt, and sends the request without one at once. The delay of the
   * attempt does not start again. */
  void answerRedirect(ISessionOutbox& outbox);

  /** @brief Counts @p elapsed, and sends the request again once
   * network::CONNECTION_REQUEST_INTERVAL passed since the previous one.
   * @returns TimedOut once CONNECTION_ATTEMPT_TIMEOUT passed since begin(),
   * and then sends nothing. */
  [[nodiscard]] AttemptProgress advance(engine::Duration elapsed,
                                        ISessionOutbox& outbox);

  /** @returns The nickname the attempt asks to join as. */
  [[nodiscard]] const std::string& nickname() const;

 private:
  std::string nickname_;
  network::GameChoice game_{network::ANY_GAME_CHOICE};
  std::uint64_t cookie_{0};
  bool redirected_{false};
  engine::Duration sinceRequest_{};
  engine::Duration sinceBegin_{};
};

}  // namespace rtype::client
