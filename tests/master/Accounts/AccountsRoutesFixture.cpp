#include "AccountsRoutesFixture.hpp"
#include <httplib.h>
#include <string_view>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"

const RunningMaster& AccountsRoutesFixture::master() const { return master_; }

httplib::Result AccountsRoutesFixture::send(const RouteCall& call) const {
  return sendRouteCall(master_.port(), call);
}

httplib::Result AccountsRoutesFixture::createAce() const {
  return send(credentialsCall(USERS_URL, aceCredentials()));
}

httplib::Result AccountsRoutesFixture::signInAsAce(
    std::string_view password) const {
  return send(credentialsCall(SESSIONS_URL, aceWith(password)));
}

httplib::Result AccountsRoutesFixture::readOwnUser(
    std::string_view token) const {
  return send(readOwnUserCall(token));
}

httplib::Result AccountsRoutesFixture::deleteSession(
    std::string_view token) const {
  return send(deleteSessionCall(token));
}
