/**
 * License: Apache 2.0 with LLVM Exception or GPL v3
 *
 * Author: Jesse Laning
 */

#include <iostream>
#include <iterator>

#include "argparse.h"

using namespace argparse;

int main(int argc, const char* argv[]) {
  ArgumentParser parser("example", "Argument parser example");
  parser.add_argument()
      .names({"-v", "--verbose"})
      .description("verbose level")
      .required(true);
  parser.add_argument("-t", "--test", "test", true)
      .position(ArgumentParser::Argument::Position::LAST);
  parser.add_argument("-d", "--dtest", "dtest", true).position(0);
  parser.enable_help();
  auto err = parser.parse(argc, argv);
  if (err) {
    std::cout << err << std::endl;
    return -1;
  }

  if (parser.exists("help")) {
    parser.print_help();
    return 0;
  }

  if (parser.exists("v")) {
    switch (parser.get<unsigned int>("v")) {
      case 2:
        std::cout << "an even more verbose string" << std::endl;
#ifdef __clang__
        [[clang::fallthrough]];
#endif
        // fall through
      case 1:
        std::cout << "a verbose string" << std::endl;
#ifdef __clang__
        [[clang::fallthrough]];
#endif
        // fall through
      default:
        std::cout << "some verbosity" << std::endl;
    }
  }

  if (parser.exists("test")) {
    std::cout << parser.get<std::string>("test") << std::endl;
  }

  if (parser.exists("dtest")) {
    std::cout << parser.get<std::string>("dtest") << std::endl;
  }
}
