#pragma once

#include <stop_token>
#include <string>
#include <thread>
#include "BoundedQueue.hpp"
#include "ConsoleChunk.hpp"
#include "ConsoleLineSplitter.hpp"
#include "IConsoleInput.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief Reads what is typed in the console on the AdminConsole thread, which
 * it owns, and hands each line to the simulation thread through a queue: the
 * commands touch the games, which only the simulation thread may.
 *
 * The thread starts with the reader and stops when the reader is destroyed:
 * the read in progress is woken (see IConsoleInput::read()). When the input
 * ends (no terminal, or its input closed), the last line without an end of
 * line is handed over, the journal says the console reads nothing more, and
 * the thread ends; the server goes on.
 */
class ConsoleReader {
 public:
  /** @param input What the thread reads; it must outlive the reader.
   * @param lines Where the thread hands the lines; it must outlive the
   * reader. */
  ConsoleReader(IConsoleInput& input,
                concurrency::BoundedQueue<std::string>& lines);

  ConsoleReader(const ConsoleReader&) = delete;
  ConsoleReader& operator=(const ConsoleReader&) = delete;
  ConsoleReader(ConsoleReader&&) = delete;
  ConsoleReader& operator=(ConsoleReader&&) = delete;
  ~ConsoleReader() = default;

 private:
  void run(const std::stop_token& stop);
  /** @brief Hands the lines of @p chunk over, and writes in the journal what
   * could not be. */
  void handOver(ConsoleChunk chunk);

  IConsoleInput* input_;
  concurrency::BoundedQueue<std::string>* lines_;
  ConsoleLineSplitter splitter_;
  logging::Logger logger_;
  std::jthread console_;
};

}  // namespace rtype::server
