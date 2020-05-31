# argparse
A simple header only command line argument parser

|Master|Develop|
|:-:|:-:|
|[![Build Status](https://travis-ci.com/jamolnng/argparse.svg?branch=master)](https://travis-ci.com/jamolnng/argparse)|[![Build Status](https://travis-ci.com/jamolnng/argparse.svg?branch=develop)](https://travis-ci.com/jamolnng/argparse)|

## Table of Contents
- [Building With Git and CMake](#Building-With-Git-and-CMake)
    * [Make](#build-make)
    * [VSCode and CMake Tools](#build-vscode)
    * [Visual Studio](#build-vsc)
- [Example](#Example)
- [Usage](#Usage)
- [Running Tests](#Running-Tests)
    * [Make](#test-make)
    * [VSCode and CMake Tools](#test-vscode)
    * [Visual Studio](#test-vsc)
- [Contributing](#Contributing)
- [License](#License)

## Building With Git and CMake
[Git](https://git-scm.com) and [CMake](https://cmake.org/)
### <a name="build-make"></a>Make
[Make](https://www.gnu.org/software/make/)
```bash
git clone https://github.com/jamolnng/argparse.git
cd argparse
mkdir build && cd build
cmake ..
make
```
### <a name="build-vscode"></a>VSCode and CMake Tools
[VSCode](https://code.visualstudio.com/) and [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

TODO
### <a name="build-vsc"></a>Visual Studio
[Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)

TODO
## Example
```cpp
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
```
Example output:
```
> program "something" -v 2 "something else"
an even more verbose string
a verbose string
some verbosity
something else
something

> program "something" -v=1 "something else"
a verbose string
some verbosity
something else
something

> program "something" --verbose "something else"
some verbosity
something else
something

> program -h
Usage: example [options...] [t] [d]
Options:
    -v, --verbose          verbose level           (Required)
    -t, --test             test                    (Required)
    -d, --dtest            dtest                   (Required)
    -h, --help             Shows this page      

> program
Required argument not found: -v
```
## Usage
TODO
## TODO
- [ ] Positional argumeents
- [ ] More error checking
- [ ] Think of more things to do
## Running Tests
### <a name="test-make"></a>Make
```bash
make test
```
###
### <a name="test-vscode"></a>VSCode and CMake Tools
TODO
### <a name="test-vsc"></a>Visual Studio
TODO

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[Apache-2.0-with-LLVM-Exception](./LICENSE.Apache-2.0-with-LLVM-Exception) or [GPL-3.0](./LICENSE.GPL-3.0)
