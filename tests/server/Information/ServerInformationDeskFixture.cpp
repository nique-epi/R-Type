#include "ServerInformationDeskFixture.hpp"
#include <chrono>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerInformationDeskParts.hpp"
#include "ServerInformationDeskTestConstants.hpp"

ServerInformationDeskFixture::ServerInformationDeskFixture() {
  answerAs(rtype::server::testing::DESK_SERVER_NAME);
}

void ServerInformationDeskFixture::answerAs(std::string_view name) {
  desk_.emplace(rtype::server::ServerInformationDeskParts{
      .name = std::string{name},
      .gameVersion = std::string{rtype::server::testing::DESK_GAME_VERSION},
      .send =
          [this](rtype::network::OutgoingDatagram datagram) {
            sent_.push_back(std::move(datagram));
          },
      .occupancy = [] { return rtype::server::testing::DESK_OCCUPANCY; }});
}

void ServerInformationDeskFixture::answer(
    const std::vector<rtype::network::IncomingDatagram>& datagrams,
    std::chrono::nanoseconds now) {
  desk_->answer(datagrams, now);
}

const std::vector<rtype::network::OutgoingDatagram>&
ServerInformationDeskFixture::sent() const {
  return sent_;
}
