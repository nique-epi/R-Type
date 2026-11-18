#include "MasterAccountClientFixture.hpp"
#include <vector>
#include "FakeAccountMaster.hpp"
#include "MasterAccountClient.hpp"
#include "RecordedRequest.hpp"

MasterAccountClientFixture::MasterAccountClientFixture()
    : client_(master_.url()) {}

FakeAccountMaster& MasterAccountClientFixture::master() { return master_; }

rtype::network::MasterAccountClient& MasterAccountClientFixture::client() {
  return client_;
}

RecordedRequest MasterAccountClientFixture::lastRequest() const {
  const std::vector<RecordedRequest> requests = master_.requests();
  return requests.empty() ? RecordedRequest{} : requests.back();
}
