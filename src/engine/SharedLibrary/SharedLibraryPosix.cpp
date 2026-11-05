#include <dlfcn.h>
#include <filesystem>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "PathText.hpp"
#include "SharedLibrary.hpp"
#include "SharedLibraryConstants.hpp"

namespace rtype::engine {

namespace {

/**
 * @returns What dlopen() last reported, read on the thread that called it:
 * dlerror() returns the text of the last failure of the calling thread, or
 * nullptr when there is none. The program loads its libraries before its
 * modules start any thread, so no other call to dlerror() runs meanwhile.
 */
std::string lastError() {
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char* const reason = dlerror();
  return reason == nullptr ? std::string{UNKNOWN_SYSTEM_REASON}
                           : std::string{reason};
}

}  // namespace

SharedLibrary::SharedLibrary(const std::filesystem::path& file)
    : handle_(dlopen(file.c_str(), RTLD_NOW | RTLD_LOCAL)) {
  if (handle_ == nullptr) {
    throw SharedLibraryOpenException(genericText(file), lastError());
  }
}

SharedLibrary::~SharedLibrary() { static_cast<void>(dlclose(handle_)); }

void* SharedLibrary::symbol(const std::string& name) const {
  return dlsym(handle_, name.c_str());
}

std::string_view SharedLibrary::fileSuffix() { return POSIX_LIBRARY_SUFFIX; }

}  // namespace rtype::engine
