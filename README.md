# argparse
A simple header only command line argument parser

## Usage
Here is a simple example

```cpp
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
  } catch (const ArgumentParser::ArgumentNotFound& ex) {
    std::cout << ex.what() << std::endl;
    return 0;
  }
  if (parser.is_help()) return 0;
  std::cout << parser.get<int>("a") << std::endl;
  std::cout << (parser.get<bool>("flag") ? "true" : "false") << std::endl;
  std::cout << (parser.get<bool>("d") ? "true" : "false") << std::endl;
  auto v = parser.getv<double>("v");
  std::copy(v.begin(), v.end(), std::ostream_iterator<double>(std::cout, " "));
  if (v.size()) std::cout << std::endl;
  return 0;
}
```

## Compiling
Just add `argparse.h` to your include path. Requires compiler support for `<regex>` (ex: g++>=4.9)
