#pragma once

#include "AccountsService.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"

namespace rtype::master {

/**
 * @brief The routes of the accounts and of their sessions: what reads their
 * request and writes their answer. Each route is
 * network::MasterRoute::httpsOnly, so a request that did not reach the master
 * over HTTPS gets 403 from the MasterHttpServer before it reaches the
 * controller. An account is named by the token of its session, never by a
 * number in the path. Every refusal is a problem document: 400 naming the
 * faulty member, 401 without a valid session (with `WWW-Authenticate: Bearer`),
 * 409 for a nickname taken, 429 over a quota.
 */
class AccountsController {
 public:
  /** @param service Applies the rules of the accounts; it must outlive the
   * controller. */
  explicit AccountsController(AccountsService& service);

  /**
   * @returns The answer of `POST /api/users`: 201, `Location` naming the
   * account, and the account; 409 for a nickname taken, 429 when the address
   * created too many accounts.
   */
  [[nodiscard]] network::ApiResponse createUser(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/users/me`: 200 and the account of the
   * session of the token; 401 for no session. */
  [[nodiscard]] network::ApiResponse readOwnUser(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `POST /api/sessions`: 201, `Location` naming the
   * session of the token, and the session with its token, shown this once; 401
   * for a wrong nickname or password, 429 when the address failed too many
   * sign-ins.
   */
  [[nodiscard]] network::ApiResponse createSession(
      const network::ApiRequest& request);

  /** @returns The answer of `DELETE /api/sessions/me`: 204 whether a session
   * had the token or not, 401 without a token. */
  [[nodiscard]] network::ApiResponse deleteOwnSession(
      const network::ApiRequest& request);

  /** @returns The answer of every route of the accounts on a master started
   * without a key of the passwords: 503. */
  [[nodiscard]] static network::ApiResponse closedAnswer(
      const network::ApiRequest& request);

 private:
  AccountsService& service_;
};

}  // namespace rtype::master
