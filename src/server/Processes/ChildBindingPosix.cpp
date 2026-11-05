#include <boost/process/v2/default_launcher.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cerrno>
#include <csignal>
#include <filesystem>
#include "ChildBinding.hpp"
#if defined(__linux__)
#include <sys/prctl.h>
#endif

namespace rtype::server {

ChildBinding::ChildBinding() = default;

ChildBinding::~ChildBinding() = default;

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
boost::system::error_code ChildBinding::on_exec_setup(
    [[maybe_unused]] boost::process::v2::default_process_launcher& launcher,
    [[maybe_unused]] const std::filesystem::path& program,
    [[maybe_unused]] const char* const* arguments) {
#if defined(__linux__)
  if (::prctl(PR_SET_PDEATHSIG, SIGTERM) == -1) {
    return boost::system::error_code{errno, boost::system::system_category()};
  }
#endif
  return {};
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool ChildBinding::lastProcessWasBound() const { return true; }

}  // namespace rtype::server
