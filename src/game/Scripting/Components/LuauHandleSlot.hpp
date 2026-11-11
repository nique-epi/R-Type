#pragma once

namespace rtype::game {

/** @brief What the userdata of a handle holds: the reference of its hidden
 * table in the registry of the machine, and whether its body still lives. A
 * handle a script kept after its body ended reads `alive` false and refuses
 * every field, so it never reaches a table the reference may name since. */
struct LuauHandleSlot {
  int hiddenTable{0};
  bool alive{false};
};

}  // namespace rtype::game
