#include <windows.h>
#include <boost/process/v2/process.hpp>
#include <boost/system/detail/error_code.hpp>
#include "ChildBinding.hpp"
#include "ServerException.hpp"

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

void ChildBinding::bind(boost::process::v2::process& process) {
  if (::AssignProcessToJobObject(job_, process.native_handle()) == 0) {
    boost::system::error_code ignored;
    process.terminate(ignored);
    throw ChildBindingException("AssignProcessToJobObject");
  }
}

}  // namespace rtype::server
