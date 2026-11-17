#include "MasterDeviceLinkClient.hpp"
#include <string>
#include "CreatedDeviceLink.hpp"
#include "CreatedSession.hpp"
#include "DeviceLinkState.hpp"
#include "MasterMessageText.hpp"
#include "MasterRequest.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::network {

MasterDeviceLinkClient::MasterDeviceLinkClient(const std::string& url)
    : transport_(url) {}

CreatedDeviceLink MasterDeviceLinkClient::createDeviceLink() {
  return createdDeviceLinkFromText(
      transport_.send(masterRequestOf(CREATE_DEVICE_LINK_ROUTE,
                                      apiPathOf(CREATE_DEVICE_LINK_ROUTE)),
                      std::string{}));
}

DeviceLinkState MasterDeviceLinkClient::readOwnDeviceLink(
    const std::string& secret) {
  return deviceLinkStateFromText(
      transport_.send(masterRequestOf(READ_OWN_DEVICE_LINK_ROUTE,
                                      apiPathOf(READ_OWN_DEVICE_LINK_ROUTE)),
                      secret));
}

void MasterDeviceLinkClient::deleteOwnDeviceLink(const std::string& secret) {
  static_cast<void>(
      transport_.send(masterRequestOf(DELETE_OWN_DEVICE_LINK_ROUTE,
                                      apiPathOf(DELETE_OWN_DEVICE_LINK_ROUTE)),
                      secret));
}

CreatedSession MasterDeviceLinkClient::createSessionOfDeviceLink(
    const std::string& secret) {
  return createdSessionFromText(transport_.send(
      masterRequestOf(CREATE_SESSION_ROUTE, apiPathOf(CREATE_SESSION_ROUTE)),
      secret));
}

void MasterDeviceLinkClient::deleteOwnSession(const std::string& token) {
  static_cast<void>(
      transport_.send(masterRequestOf(DELETE_OWN_SESSION_ROUTE,
                                      apiPathOf(DELETE_OWN_SESSION_ROUTE)),
                      token));
}

}  // namespace rtype::network
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
