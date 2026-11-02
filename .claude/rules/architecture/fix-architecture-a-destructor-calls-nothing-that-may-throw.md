---
description: "A destructor calls nothing that may throw: every call it makes is followed down to the library before it is written"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A destructor calls nothing that may throw — every call it makes is followed down to the library

Why: `~DelayedDelivery()` called `close()`, whose `asio::steady_timer::cancel()` throws on failure; `bugprone-exception-escape` failed the tidy gate, since an exception that leaves a destructor calls `std::terminate`.

## Rule

1. **Before writing a destructor body, follow each call it makes into the bodies it reaches**, library calls included, and list those that may throw. A member function that looks harmless (`close()`) may reach one.
2. **Use the overload that reports through an `std::error_code&`** when the library has one: `socket.close(ignored)`, as `~UdpSocket` does.
3. **When there is none, the destructor only does what cannot throw** (set a flag the handlers read) and leaves the throwing call to a function the owner calls before. This project builds Asio with `ASIO_NO_DEPRECATED`, which removes `steady_timer::cancel(error_code&)`: only the throwing `cancel()` is left.
4. **`bugprone-exception-escape` only sees the bodies of the file it lints**: a destructor in another file that calls the same function is not reported. Search the callers (`git grep -n '<function>()' -- src`) and report those the change does not touch.

## Example

- ❌ **Before (wrong)**: `DelayedDelivery::~DelayedDelivery() { close(); }`, with `close()` calling `timer.cancel()` → `an exception may be thrown in function '~DelayedDelivery' which should not throw exceptions`.
- ✅ **After (right)**: `DelayedDelivery::~DelayedDelivery() { state_->closed = true; }`: the handler of the timer reads the flag and delivers nothing; `~SimulatedNetworkSocket`, which still calls `close()`, is reported in the pull request.
