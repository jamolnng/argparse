#include <iostream>
#include "argparse.h"

int main(int argc, char* argv[]) {
  // run as: [program name] -a 1 -sdf --flag -v 1 2.7 3 4 9 8.12 87 6
  ArgumentParser parser("Argument parser example");
  parser.add_argument("-a", "an integer");
  parser.add_argument("-s", "an combined flag", true);
  parser.add_argument("-d", "an combined flag", true);
  parser.add_argument("-f", "an combined flag", true);
  parser.add_argument("--flag", "a flag");
  parser.add_argument("-v", "a vector", true);
  try {
    parser.parse(argc, argv);
  } catch (const ArgumentParser::ArgumentException& ex) {
    std::cout << ex.what() << std::endl;
    return 0;
  }
  if (parser.is_help()) return 0;
  std::cout << parser.get<int>("a") << std::endl;
  std::cout << (parser.get<bool>("flag") ? "true" : "false") << std::endl;
  std::cout << (parser.get<bool>("d") ? "true" : "false") << std::endl;
  for (auto i : parser.getv<double>("v")) std::cout << i << " : ";
  std::cout << std::endl;
  return 0;
}