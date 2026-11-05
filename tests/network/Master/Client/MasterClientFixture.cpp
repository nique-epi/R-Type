#include "MasterClientFixture.hpp"
#include <functional>
#include <string>
#include "CreatedServer.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "MasterClientTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkException.hpp"
#include "ServerCreation.hpp"

rtype::network::MasterClient MasterClientFixture::clientWith(
    const std::string& key) const {
  return rtype::network::MasterClient{
      rtype::network::MasterAccess{.url = master_.url(), .key = key}};
}

rtype::network::CreatedServer MasterClientFixture::createdParis() const {
  return clientWith(std::string{})
      .createServer(rtype::network::ServerCreation{
          .name = std::string{TEST_SERVER_NAME}});
}

rtype::network::MasterRefusedException MasterClientFixture::refusalOf(
    const std::function<void()>& call) {
  try {
    call();
  } catch (const rtype::network::MasterRefusedException& refusal) {
    return refusal;
  }
  return rtype::network::MasterRefusedException{NO_REFUSAL, std::string{}};
}
