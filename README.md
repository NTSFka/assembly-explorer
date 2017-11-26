
# Assembly Explorer

GUI tool for exploring assembly code from compiled binaries. The inspiration
is taken from [Compiler Explorer](https://godbolt.org) which is great for small pieces of code but
unsuitable for whole projects with dependencies.

The tool pass the given binary to objdump tool and parses the result output
and create browsable structure.

## Requirements

* C++ compiler
* Qt 5
* qmake
* objdump tool for obtaining assembly code.
