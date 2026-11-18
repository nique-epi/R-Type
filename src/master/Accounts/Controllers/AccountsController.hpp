#pragma once

#include "AccountsService.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "DeviceLinksService.hpp"
#include "SiteVisitors.hpp"

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
 *
 * A creation and a sign-in are counted under the visitor the web site names
 * with the site key (SiteVisitors::addressOf()), under the address they came
 * from otherwise.
 */
class AccountsController {
 public:
  /**
   * @param service Applies the rules of the accounts; it must outlive the
   * controller.
   * @param links Trades an approved device link for a session; it must
   * outlive the controller.
   * @param siteVisitors Who a creation or a sign-in is counted under; it must
   * outlive the controller.
   */
  AccountsController(AccountsService& service, DeviceLinksService& links,
                     const SiteVisitors& siteVisitors);

  /**
   * @returns The answer of `POST /api/users`: 201, `Location` naming the
   * account, and the account; 409 for a nickname taken, 429 when the address
   * or the visitor created too many accounts.
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
   * for a wrong nickname or password, 429 when the address or the visitor
   * failed too many sign-ins. A request with no body and the secret of a
   * device link as Bearer trades the link instead: 201 and the session of the
   * account that approved it; 401 for no link of that secret, 409 while it
   * waits, 403 once refused, 410 once expired.
   */
  [[nodiscard]] network::ApiResponse createSession(
      const network::ApiRequest& request);

  /** @returns The answer of `DELETE /api/sessions/me`: 204 whether a session
   * had the token or not, 401 without a token. */
  [[nodiscard]] network::ApiResponse deleteOwnSession(
      const network::ApiRequest& request);

 private:
  AccountsService& service_;
  DeviceLinksService& links_;
  const SiteVisitors& siteVisitors_;
};

}  // namespace rtype::master
