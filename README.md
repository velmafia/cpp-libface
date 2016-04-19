**INTRO**

Purpose of this project is:

- refactor original cpp-libface executable to library without dependencies on libuv and http-server
- make it working under Windows, OS X and Linux

**BUILD**

_Windows:_

- `git submodules init`, `git submodules update`, open `mman-win32` project and build it
- put `mman.lib` to the `libs/` directory
- if you have Qt installed, open `libface-project/libface.pro` and build for Debug/Release
- otherwise, open Visual Studio command line for root directory or this repo
- run `nmake -f Makefile.win lib-face` in order to build libface

_Linux/OS X:_

- if you have Qt installed, open `libface-project/libface.pro` and build for Debug/Release
- otherwise run `make lib-face` in the terminal in root directory of this repo

**ORIGINAL cpp-libface**

See ORIGINAL_README.md of cpp-libface.
