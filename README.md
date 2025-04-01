# Compiling
If you want to use [cmake](https://cmake.org/).

#### IMPORTANT
Before building, change `include/constants.hpp.example` to `include/constants.hpp` and edit the fields in to match your configuration.

---
In order to build this project, use:
```
cmake -B build -G <build type>
```
The build type used during creation was `Ninja`.

To integrate with your IDE or LSP, copy the `compile_commands.json` into the project path
