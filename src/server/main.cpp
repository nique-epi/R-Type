#include <asio.hpp>

int main() {
  asio::io_context context;
  context.run();
  return 0;
}
