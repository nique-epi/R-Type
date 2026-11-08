#pragma once

#include <boost/asio/readable_pipe.hpp>
#include <thread>
#include "ChildOutputLines.hpp"

namespace rtype::server {

/**
 * @brief Relays what a child process writes on one of its outputs to the
 * standard error of this process, through logging::writeStandardErrorLine(), on
 * a thread it owns: the lines each read completes, as one text. The admin
 * console then writes each line above the line being typed, where a child that
 * wrote to the terminal itself would cut it.
 *
 * The thread reads until the child closes its end of the pipe, when it ends or
 * is killed, a read a signal interrupted being tried again, and the relay
 * joins it when destroyed: destroy it once the child
 * ended or was killed, as BoostChildProcess does. The reads never stop: a full
 * pipe would block a child that writes a lot. The io_context of the pipe must
 * outlive the relay.
 */
class ChildOutputRelay {
 public:
  /** @param pipe The end of the pipe this process reads, the child holding the
   * other. */
  explicit ChildOutputRelay(boost::asio::readable_pipe pipe);

  ChildOutputRelay(const ChildOutputRelay&) = delete;
  ChildOutputRelay& operator=(const ChildOutputRelay&) = delete;
  ChildOutputRelay(ChildOutputRelay&&) = delete;
  ChildOutputRelay& operator=(ChildOutputRelay&&) = delete;
  ~ChildOutputRelay() = default;

 private:
  void run();

  boost::asio::readable_pipe pipe_;
  ChildOutputLines lines_;
  std::jthread reader_;
};

}  // namespace rtype::server
