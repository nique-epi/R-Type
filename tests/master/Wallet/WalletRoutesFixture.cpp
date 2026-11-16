#include "WalletRoutesFixture.hpp"
#include <httplib.h>
#include <string>
#include <vector>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"
#include "WalletRoutesTestConstants.hpp"

WalletRoutesFixture::WalletRoutesFixture()
    : master_(std::vector<std::string>{std::string{STARTING_KOPECKS_LINE}}) {
  static_cast<void>(send(credentialsCall(USERS_URL, aceCredentials())));
  token_ = memberIn(send(credentialsCall(SESSIONS_URL, aceCredentials())),
                    TOKEN_MEMBER_NAME);
}

const std::string& WalletRoutesFixture::token() const { return token_; }

httplib::Result WalletRoutesFixture::send(const RouteCall& call) const {
  return sendRouteCall(master_.port(), call);
}
