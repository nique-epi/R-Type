#include "MasterDeviceLinkFlowFixture.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "CannedAnswer.hpp"
#include "CreatedDeviceLink.hpp"
#include "CreatedSession.hpp"
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkStatus.hpp"
#include "FakeAccountMaster.hpp"
#include "MasterDeviceLinkFlow.hpp"
#include "MasterDeviceLinkFlowTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "RecordedRequest.hpp"
#include "User.hpp"
#include "UserRole.hpp"
#include "ValueSoon.hpp"

namespace rtype::client::testing {

MasterDeviceLinkFlowFixture::MasterDeviceLinkFlowFixture() {
  master_.answer(
      std::string{CREATE_LINK_ROUTE},
      CannedAnswer{
          .status = FLOW_CREATED_STATUS,
          .body = network::createdDeviceLinkText(network::CreatedDeviceLink{
              .secret = std::string{FLOW_SECRET},
              .userCode = std::string{FLOW_USER_CODE},
              .verificationUri = std::string{FLOW_PAGE},
              .verificationUriComplete = std::string{FLOW_PAGE_WITH_CODE},
              .expiresAt = FLOW_EXPIRES_AT,
              .interval = FLOW_INTERVAL})});
  master_.answer(
      std::string{CREATE_SESSION_ROUTE},
      CannedAnswer{
          .status = FLOW_CREATED_STATUS,
          .body = network::createdSessionText(network::CreatedSession{
              .token = std::string{FLOW_TOKEN},
              .expiresAt = FLOW_EXPIRES_AT,
              .user = network::User{.id = FLOW_ACE_ID,
                                    .nickname = std::string{FLOW_ACE_NICKNAME},
                                    .role = network::UserRole::Player}})});
  showLinkAs(network::DeviceLinkStatus::Pending);
  flow_.start(master_.url());
}

void MasterDeviceLinkFlowFixture::TearDown() { flow_.stop(); }

FakeAccountMaster& MasterDeviceLinkFlowFixture::master() { return master_; }

MasterDeviceLinkFlow& MasterDeviceLinkFlowFixture::flow() { return flow_; }

void MasterDeviceLinkFlowFixture::showLinkAs(network::DeviceLinkStatus status) {
  master_.answer(std::string{READ_OWN_LINK_ROUTE},
                 CannedAnswer{.status = FLOW_OK_STATUS,
                              .body = network::deviceLinkStateText(
                                  network::DeviceLinkState{
                                      .userCode = std::string{FLOW_USER_CODE},
                                      .status = status,
                                      .createdAt = FLOW_CREATED_AT,
                                      .expiresAt = FLOW_EXPIRES_AT,
                                      .interval = FLOW_INTERVAL})});
}

std::optional<DeviceLinkReading> MasterDeviceLinkFlowFixture::readingSoon(
    std::uint64_t ask, DeviceLinkPhase phase) const {
  return valueSoon(FLOW_READING_DEADLINE,
                   [this, ask, phase]() -> std::optional<DeviceLinkReading> {
                     std::optional<DeviceLinkReading> reading = flow_.reading();
                     if (reading.has_value() && reading->ask == ask &&
                         reading->phase == phase) {
                       return reading;
                     }
                     return std::nullopt;
                   });
}

bool MasterDeviceLinkFlowFixture::forgottenSoon() const {
  return valueSoon(FLOW_READING_DEADLINE,
                   [this]() -> std::optional<bool> {
                     for (const RecordedRequest& request : master_.requests()) {
                       if (request.method == DELETE_METHOD_NAME &&
                           request.path == OWN_LINK_PATH &&
                           request.authorization == FLOW_BEARER) {
                         return true;
                       }
                     }
                     return std::nullopt;
                   })
      .value_or(false);
}

}  // namespace rtype::client::testing
