#include "BoostChildProcess.hpp"
#include <boost/process/v2/process.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <optional>
#include <utility>
#include "ProcessExit.hpp"

namespace rtype::server {

BoostChildProcess::BoostChildProcess(boost::process::v2::process process)
    : process_(std::move(process)) {}

BoostChildProcess::~BoostChildProcess() {
  boost::system::error_code ignored;
  if (process_.running(ignored)) {
    process_.terminate(ignored);
  }
}

void BoostChildProcess::terminate() {
  boost::system::error_code ignored;
  process_.terminate(ignored);
}

std::optional<ProcessExit> BoostChildProcess::exited() {
  boost::system::error_code ignored;
  if (process_.running(ignored)) {
    return std::nullopt;
  }
  return ProcessExit{.code = process_.exit_code()};
}

std::int64_t BoostChildProcess::processId() const {
  return static_cast<std::int64_t>(process_.id());
}

}  // namespace rtype::server
