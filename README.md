Purpose of this project is:

- to convert cpp-libface to library without dependencies on libuv and http-server
- make it working under Windows

**BUILD**

Windows:

- `git submodules init`, `git submodules update`, open `mman-win32` project and build it
- open Visual Studio command line for root directory or this repo
- run `nmake -f Makefile.win lib-face` in order to build libface

Linux/OS X:

- run `make lib-face` in the terminal in root directory of this repo

See ORIGINAL_README.md of cpp-libface.
