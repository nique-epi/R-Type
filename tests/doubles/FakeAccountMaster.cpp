#include "FakeAccountMaster.hpp"
#include <httplib.h>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "CannedAnswer.hpp"
#include "FakeAccountMasterConstants.hpp"
#include "RecordedRequest.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
FakeAccountMaster::FakeAccountMaster() {
  const auto serve = [this](const httplib::Request& request,
                            httplib::Response& response) {
    this->serve(request, response);
  };
  const std::string everyPath{EVERY_PATH};
  server_.Get(everyPath, serve);
  server_.Put(everyPath, serve);
  server_.Post(everyPath, serve);
  server_.Delete(everyPath, serve);
  port_ = server_.bind_to_any_port(std::string{ACCOUNT_MASTER_HOST});
  listener_ = std::thread{[this] { server_.listen_after_bind(); }};
  server_.wait_until_ready();
}

// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)

FakeAccountMaster::~FakeAccountMaster() {
  server_.stop();
  listener_.join();
}

void FakeAccountMaster::answer(const std::string& route, CannedAnswer answer) {
  const std::scoped_lock lock{mutex_};
  answers_[route] = std::move(answer);
}

std::string FakeAccountMaster::url() const {
  return std::string{ACCOUNT_MASTER_SCHEME} + std::string{ACCOUNT_MASTER_HOST} +
         std::string{ACCOUNT_MASTER_PORT_SEPARATOR} + std::to_string(port_);
}

std::vector<RecordedRequest> FakeAccountMaster::requests() const {
  const std::scoped_lock lock{mutex_};
  return requests_;
}

void FakeAccountMaster::serve(const httplib::Request& request,
                              httplib::Response& response) {
  const std::scoped_lock lock{mutex_};
  requests_.push_back(RecordedRequest{
      .method = request.method,
      .path = request.path,
      .authorization =
          request.get_header_value(std::string{AUTHORIZATION_HEADER_NAME}),
      .body = request.body});
  const auto answer = answers_.find(
      request.method + std::string{ROUTE_METHOD_SEPARATOR} + request.path);
  if (answer == answers_.end()) {
    response.status = UNANSWERED_ROUTE_STATUS;
    return;
  }
  response.status = answer->second.status;
  if (!answer->second.body.empty()) {
    response.set_content(answer->second.body,
                         std::string{ACCOUNT_MASTER_CONTENT_TYPE});
  }
}
