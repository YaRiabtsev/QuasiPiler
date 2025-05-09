![QuasiPiler Logo Light](logo/llogo.svg#gh-light-mode-only)![QuasiPiler Logo Dark](logo/dlogo.svg#gh-dark-mode-only)
## _— the Hunchback Dragon of Compilers_

[//]: # ([![version]&#40;https://img.shields.io/github/v/release/YaRiabtsev/QuasiPiler?include_prereleases&#41;]&#40;https://github.com/YaRiabtsev/QuasiPiler/releases/latest&#41;)
[![codecov](https://codecov.io/gh/YaRiabtsev/QuasiPiler/graph/badge.svg?token=MCNEJFWMDU)](https://codecov.io/gh/YaRiabtsev/QuasiPiler)
[![license](https://img.shields.io/github/license/YaRiabtsev/QuasiPiler?color=e6e6e6)](https://github.com/YaRiabtsev/QuasiPiler/blob/master/license)

> “A one-eyed transpiler is much more incomplete than a blind transpiler, for he knows what it is that’s lacking.”  
> — Victor-Marie from Gugle

This repo is my sanctuary under license — it begs mercy, not stars. I’ll bell when (or if) it works.
“Documentation and Contributing” is a friendly suggestion, not a Martin Luther pinboard.

## Setup and Installation

### Requirements

* **C++20** compiler
* **cxxopts**: for command line options
* **GTest**: for unit tests
* **lcov**: for code coverage reports
* **doxygen** and **graphviz**: for generating documentation

### Building the Application

1. Build with CMake in Release mode:
    ```bash
    $ cmake -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -B build -S .
    $ cmake --build build
    ```
2. Run the Application:
    ```bash
    $ qpiler [options] <inputfile>
    ```
    * `<inputfile>`: path to your QuasiCode file

## QuasiCode Syntax

### Features in Progress

## Example Usage

## Documentation and Contributing

To build and run tests, enable debug mode, or generate coverage reports:

1. **Build with Debug and Coverage:**
   ```bash
   $ cmake -B build CMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DCOVERAGE=ON
   ```
2. **Generate Coverage Report and HTML:**
   ```bash
   $ cmake --build build --target coverage
   ```

For detailed documentation, see the [Documentation](https://yariabtsev.github.io/QuasiPiler/doc/) and for the latest
coverage report, see [Coverage](https://yariabtsev.github.io/QuasiPiler/cov/).

## License

This project is open-source and available under the MIT License.