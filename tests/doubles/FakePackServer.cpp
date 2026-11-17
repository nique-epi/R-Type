#include "FakePackServer.hpp"
#include <httplib.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include "FakePackServerConstants.hpp"
#include "LoopbackListener.hpp"
#include "ServedPack.hpp"

FakePackServer::FakePackServer(ServedPack pack) : body_(std::move(pack.body)) {
  server_.Get(pack.path, [this](const httplib::Request& request,
                                httplib::Response& response) {
    answer(request, response);
  });
  listener_.emplace(server_, LOOPBACK);
}

FakePackServer::~FakePackServer() {
  releaseTheAnswers();
  listener_.reset();
}

void FakePackServer::cutTheFirstAnswerAfter(std::size_t bytes) {
  const std::scoped_lock lock{mutex_};
  cutAfter_ = bytes;
}

void FakePackServer::answerEveryRequestWith(int status) {
  const std::scoped_lock lock{mutex_};
  status_ = status;
}

void FakePackServer::holdTheAnswers() {
  const std::scoped_lock lock{mutex_};
  held_ = true;
}

void FakePackServer::releaseTheAnswers() {
  {
    const std::scoped_lock lock{mutex_};
    held_ = false;
  }
  released_.notify_all();
}

std::uint16_t FakePackServer::port() const {
  return static_cast<std::uint16_t>(listener_.has_value() ? listener_->port()
                                                          : 0);
}

std::size_t FakePackServer::requests() const {
  const std::scoped_lock lock{mutex_};
  return requests_;
}

std::optional<std::string> FakePackServer::lastRange() const {
  const std::scoped_lock lock{mutex_};
  return lastRange_;
}

void FakePackServer::answer(const httplib::Request& request,
                            httplib::Response& response) {
  std::unique_lock lock{mutex_};
  released_.wait(lock, [this] { return !held_; });
  ++requests_;
  lastRange_ = request.has_header(RANGE_HEADER)
                   ? std::optional{request.get_header_value(RANGE_HEADER)}
                   : std::nullopt;
  if (status_.has_value()) {
    response.status = *status_;
    return;
  }
  if (!cutAfter_.has_value()) {
    response.set_content(body_, OCTET_STREAM);
    return;
  }
  const std::size_t cut = std::exchange(cutAfter_, std::nullopt).value_or(0);
  response.set_content_provider(
      body_.size(), OCTET_STREAM,
      [this, cut](std::size_t offset, std::size_t, httplib::DataSink& sink) {
        sink.write(body_.data() + offset, cut - std::min(cut, offset));
        return false;
      });
}
