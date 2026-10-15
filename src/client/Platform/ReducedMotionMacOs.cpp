#include <objc/message.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include "PlatformConstants.hpp"
#include "ReducedMotion.hpp"

namespace rtype::client {

bool systemPrefersReducedMotion() {
  Class workspaceClass = objc_getClass(MACOS_WORKSPACE_CLASS);
  if (workspaceClass == nullptr) {
    return false;
  }
  SEL sharedWorkspace = sel_registerName(MACOS_SHARED_WORKSPACE_SELECTOR);
  SEL reduceMotion = sel_registerName(MACOS_REDUCE_MOTION_SELECTOR);
  // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto sendForObject = reinterpret_cast<id (*)(id, SEL)>(objc_msgSend);
  const auto sendForBoolean = reinterpret_cast<BOOL (*)(id, SEL)>(objc_msgSend);
  id workspace =
      sendForObject(reinterpret_cast<id>(workspaceClass), sharedWorkspace);
  // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
  if (workspace == nullptr ||
      class_respondsToSelector(object_getClass(workspace), reduceMotion) ==
          NO) {
    return false;
  }
  return sendForBoolean(workspace, reduceMotion) == YES;
}

}  // namespace rtype::client
