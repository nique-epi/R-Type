// clang-format off
#include <boost/process/v2/default_launcher.hpp>
#include <windows.h>
// clang-format on
#include <filesystem>
#include <string>
#include "ChildBinding.hpp"

namespace rtype::server {

ChildBinding::ChildBinding() {
  HANDLE job = ::CreateJobObjectW(nullptr, nullptr);
  if (job == nullptr) {
    throw ChildBindingException("CreateJobObject");
  }
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
  limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  if (::SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits,
                                sizeof(limits)) == 0) {
    static_cast<void>(::CloseHandle(job));
    throw ChildBindingException("SetInformationJobObject");
  }
  job_ = job;
}

ChildBinding::~ChildBinding() {
  if (job_ != nullptr) {
    static_cast<void>(::CloseHandle(job_));
  }
}

boost::system::error_code ChildBinding::on_success(
    boost::process::v2::default_process_launcher& launcher,
    [[maybe_unused]] const std::filesystem::path& program,
    [[maybe_unused]] std::wstring& commandLine) {
  lastProcessBound_ = ::AssignProcessToJobObject(
                          job_, launcher.process_information.hProcess) != 0;
  return {};
}

bool ChildBinding::lastProcessWasBound() const { return lastProcessBound_; }

}  // namespace rtype::server
