#include <windows.h>
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
 * @returns What the system reports for GetLastError(), read on the thread that
 * called LoadLibraryExW(), or UNKNOWN_SYSTEM_REASON when the system has no text
 * for it.
 */
std::string lastError() {
  char* text = nullptr;
  // With FORMAT_MESSAGE_ALLOCATE_BUFFER, the buffer argument is the address of
  // the pointer that receives the allocated text, typed as a string pointer.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  LPSTR const buffer = reinterpret_cast<LPSTR>(&text);
  const DWORD length = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, GetLastError(), 0, buffer, 0, nullptr);
  if (length == 0 || text == nullptr) {
    return std::string{UNKNOWN_SYSTEM_REASON};
  }
  std::string reason(text, length);
  static_cast<void>(LocalFree(text));
  reason.erase(reason.find_last_not_of(SYSTEM_TEXT_TRAILING_CHARACTERS) + 1);
  return reason;
}

}  // namespace

SharedLibrary::SharedLibrary(const std::filesystem::path& file)
    : handle_(LoadLibraryExW(file.c_str(), nullptr,
                             LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
                                 LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
  if (handle_ == nullptr) {
    throw SharedLibraryOpenException(genericText(file), lastError());
  }
}

// FreeLibrary() reports whether it freed the library. A destructor cannot act
// on a failure, so the result is dropped.
SharedLibrary::~SharedLibrary() {
  static_cast<void>(FreeLibrary(static_cast<HMODULE>(handle_)));
}

void* SharedLibrary::symbol(const std::string& name) const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<void*>(
      GetProcAddress(static_cast<HMODULE>(handle_), name.c_str()));
}

std::string_view SharedLibrary::fileSuffix() { return WINDOWS_LIBRARY_SUFFIX; }

}  // namespace rtype::engine
