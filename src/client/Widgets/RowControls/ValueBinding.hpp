#pragma once

#include <functional>

namespace rtype::client {

/**
 * @brief Where a control reads and writes the value it shows. The value lives
 * in a model the screen owns: the control keeps no copy, so it always paints
 * the current value, whoever changed it.
 */
template <typename Value>
struct ValueBinding {
  std::function<Value()> read;
  std::function<void(Value)> write;

  /** @brief Writes a value only when it differs from the one read, so a
   * control never reports a change that changes nothing. */
  void change(Value value) const;
};

template <typename Value>
void ValueBinding<Value>::change(Value value) const {
  if (value != read()) {
    write(value);
  }
}

}  // namespace rtype::client
