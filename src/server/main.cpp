#include <asio/io_context.hpp>
#include <exception>
#include <iostream>

int main() {
  try {
    asio::io_context context;
    context.run();
  } catch (const std::exception& error) {
    std::cerr << "server stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
