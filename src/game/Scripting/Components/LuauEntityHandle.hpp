#pragma once

#include "LuauHandleSlot.hpp"
#include "LuauSteppedBody.hpp"

struct lua_State;

namespace rtype::game {

/**
 * @brief The handle `e` the script of one body receives: a userdata whose
 * fields reach the components of COMPONENT_KEYS while the body steps (see
 * LuauComponentFields), whose `data` is a table the script keeps for the life
 * of the body, and whose other fields read the view of the last step of the
 * body. Writing any other field raises an error of the script.
 *
 * The components of a body are only reached during its own step: the handle
 * of another body, kept in a global, raises an error there. Outside a game
 * (no components attached to the scripts, see BehaviourScripts::attachGame()),
 * `position` and `top_speed` read the view, and every write is refused. Once
 * the handle ends with its body, the handle a script kept refuses every field.
 *
 * Built and pushed only inside a protected call of its machine, since both
 * allocate. Neither copied nor moved: the userdata holds its address.
 */
class LuauEntityHandle {
 public:
  /**
   * @param machine The main state of the machine that runs the script.
   * @param steppedBody What the step in progress tells of its body; it must
   * outlive every handle of @p machine, which reaches it from its metatable.
   */
  LuauEntityHandle(lua_State* machine, LuauSteppedBody& steppedBody);
  LuauEntityHandle(const LuauEntityHandle&) = delete;
  LuauEntityHandle& operator=(const LuauEntityHandle&) = delete;
  LuauEntityHandle(LuauEntityHandle&&) = delete;
  LuauEntityHandle& operator=(LuauEntityHandle&&) = delete;
  ~LuauEntityHandle();

  /** @brief Makes the table on top of the machine, which it pops, the view
   * the handle reads, and pushes the handle. */
  void pushWithView() const;

  /** @returns The slot of the handle, which tells it from the others, as
   * LuauSteppedBody::handle names it. */
  [[nodiscard]] const LuauHandleSlot* slot() const;

 private:
  lua_State* machine_;
  LuauHandleSlot* slot_;
  int handle_;
};

}  // namespace rtype::game
