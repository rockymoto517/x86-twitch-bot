# Compiling
This projecect depends on [xmake](https://xmake.io/#/getting_started) and [vcpkg](https://github.com/microsoft/vcpkg).

### Setup
First, download the requirements using:
```
xmake require
```

Second, setup the platform and architecture you will be using. This project is meant to be used for x86 windows.
```
xmake f -m <debug|release> -p windows -a x86
```

For more information on build configuration, see:
```
xmake f -h
```

Finally, edit the fields in `src/constants.hpp` to match your configuration.

### Building
In order to build this project, use:
```
xmake
```

To run this project, use:
```
xmake run
```

To integrate with your IDE or LSP, use:
```
xmake project -k <integration> --lsp=<lsp>
```
