**INTRO**

Purpose of this project is:

- refactor original cpp-libface executable to library without dependencies on libuv and http-server
- make it working under Windows, OS X and Linux

**BUILD**

_Windows:_

- `git submodules init`, `git submodules update`, open `mman-win32` project and build it
- open Visual Studio command line for root directory or this repo
- run `nmake -f Makefile.win lib-face` in order to build libface

_Linux/OS X:_

- run `make lib-face` in the terminal in root directory of this repo

**ORIGINAL cpp-libface**

See ORIGINAL_README.md of cpp-libface.
