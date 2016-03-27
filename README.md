Purpose of this project is:

- to convert cpp-libface to library without dependencies on libuv and http-server
- make it working under Windows

**BUILD**

Windows:

- init git submodules, open mmap-win32 project and build it
- open Visual Studio command line for root directory or this repo
- run `nmake -f Makefile.win lib-face` in order to build libface

See ORIGINAL_README.md of cpp-libface.
