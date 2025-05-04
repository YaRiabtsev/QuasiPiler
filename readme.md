![QuasiPiler Logo Light](logo/llogo.svg#gh-light-mode-only)![QuasiPiler Logo Dark](logo/dlogo.svg#gh-dark-mode-only)

## _— the Hunchback Dragon of Compilers_

> “A one-eyed transpiler is much more incomplete than a blind transpiler, for he knows what it is that’s lacking.”  
> — Victor-Marie from Gugle

This repo is my sanctuary under License, begs mercy; I’ll bell when it (maybe) works. Join at your own
pace—“Documentation and Contributing” is a friendly suggestion, not Martin Luther’s Table.

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
    $ cmake -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release <path-to-project-root>
    $ cmake --build .
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
   $ cmake -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON <path-to-project-root>
   $ cmake --build .
   $ ctest
   ```
2. **Generate Coverage Report:**
   ```bash
   $ lcov --capture --directory . --output-file coverage.info --rc branch_coverage=1 --filter branch --ignore-errors gcov,inconsistent,mismatch
   ```
3. Generate HTML Coverage Report:
   ```bash
   $ lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info --rc branch_coverage=1
   $ genhtml coverage.info --demangle-cpp --branch-coverage --output-directory ../cov
   ```

For detailed documentation, see the [Documentation](https://yariabtsev.github.io/QuasiPiler/doc/) and for the latest
coverage report, see [Coverage](https://yariabtsev.github.io/QuasiPiler/cov/).

## License

This project is open-source and available under the MIT License.