#include <shellapi.h>
#include <windows.h>
#include <string>
#include "BrowserOpening.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

bool openInBrowser(const std::string& url) {
  if (!isWebAddress(url)) {
    return false;
  }
  const std::wstring address(url.begin(), url.end());
  const HINSTANCE opened =
      ShellExecuteW(nullptr, WINDOWS_OPEN_VERB, address.c_str(), nullptr,
                    nullptr, SW_SHOWNORMAL);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<INT_PTR>(opened) > SHELL_EXECUTE_FIRST_SUCCESS;
}

}  // namespace rtype::client
