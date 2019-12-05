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
## Usage
```cpp
#include "argparse.h"

#include <iostream>

int main(int argc, char* argv[]) {
  ArgumentParser parser("Argument parser example");
  parser.add_argument("-v",             // short argument
                      "--verbose",      // long argument
                      "Verbose level",  // description
                      false             // is required
  );
  parser.parse(argc, argv);

  if (parser.exists("verbose")) {
    switch (parser.get<unsigned int>("verbose")) {
      case 2:
        std::cout << "an even more verbose string" << std::endl;
      case 1:
        std::cout << "a verbose string" << std::endl;
      default:
        std::cout << "some verbosity" << std::endl;
    }
  }
}
```
Example output:
```bash
> program -v 2
an even more verbose string
a verbose string
some verbosity
> program --verbose
some verbosity
```
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
[GNU General Public License v3.0](https://github.com/jamolnng/argparse/blob/master/LICENSE)