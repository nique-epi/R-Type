#include "MacOsWorkspaceFlag.hpp"
#include <objc/message.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include "PlatformConstants.hpp"

namespace rtype::client {

bool macOsWorkspaceFlag(const char* selector) {
  Class workspaceClass = objc_getClass(MACOS_WORKSPACE_CLASS);
  if (workspaceClass == nullptr) {
    return false;
  }
  SEL sharedWorkspace = sel_registerName(MACOS_SHARED_WORKSPACE_SELECTOR);
  SEL property = sel_registerName(selector);
  // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto sendForObject = reinterpret_cast<id (*)(id, SEL)>(objc_msgSend);
  const auto sendForBoolean = reinterpret_cast<BOOL (*)(id, SEL)>(objc_msgSend);
  id workspace =
      sendForObject(reinterpret_cast<id>(workspaceClass), sharedWorkspace);
  // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
  if (workspace == nullptr ||
      class_respondsToSelector(object_getClass(workspace), property) == NO) {
    return false;
  }
  return sendForBoolean(workspace, property) == YES;
}

}  // namespace rtype::client
