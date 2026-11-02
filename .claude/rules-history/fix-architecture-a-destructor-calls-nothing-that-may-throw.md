# RULE: A destructor calls nothing that may throw — every call it makes is followed down to the library

## Context

Pull request #166 (the network simulator) was reviewed by CodeRabbit, which asked `DelayedDelivery` to stop deriving from `std::enable_shared_from_this`. Its state moved into a `DelayedDeliveryState` it holds through a `std::shared_ptr`, shared with the handler of its timer. A simplification pass then had the simulated socket hold its sending `DelayedDelivery` by value, and gave `DelayedDelivery` a destructor that closes it, so an owner could no longer forget to.

## Mistake

`DelayedDelivery::~DelayedDelivery() { close(); }`, where `close()` sets the closed flag and calls `asio::steady_timer::cancel()`. The local clang-tidy run before the push reported `bugprone-exception-escape` as an error: `cancel()` calls `asio::detail::throw_error(ec, "cancel")`, so an exception may leave the destructor, which calls `std::terminate`. Asio's timer service never fails a cancel in practice, but the gate failed, and nothing in the code proved it.

## Root cause

- `close()` was read as a harmless call of the class itself; the library call it reaches was not opened.
- The project builds Asio with `ASIO_NO_DEPRECATED`, which removes the `cancel(asio::error_code&)` overload: the reflex of `~UdpSocket`, which closes its socket with an ignored `std::error_code`, has no equivalent for a timer.
- `~SimulatedNetworkSocket` already called `close()` on both of its deliveries. `bugprone-exception-escape` did not report it, since `close()` is defined in another file: the same risk went unseen until the destructor sat in the file that defines `close()`.

## Rule

1. Before writing a destructor body, follow each call it makes into the bodies it reaches, library calls included, and list those that may throw.
2. Use the overload that reports through an `std::error_code&` when the library has one.
3. When there is none, the destructor only does what cannot throw and leaves the throwing call to a function the owner calls before.
4. `bugprone-exception-escape` only sees the bodies of the file it lints: search the other callers and report those the change does not touch.

## Example

- ❌ `DelayedDelivery::~DelayedDelivery() { close(); }`.
- ✅ `DelayedDelivery::~DelayedDelivery() { state_->closed = true; }`; `~SimulatedNetworkSocket`, which still calls `close()`, reported.
