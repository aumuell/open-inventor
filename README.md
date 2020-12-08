![make build status](https://github.com/aumuell/open-inventor/workflows/make/badge.svg)
![CMake build status](https://github.com/aumuell/open-inventor/workflows/CMake/badge.svg)
![macOS build status](https://github.com/aumuell/open-inventor/workflows/macOS/badge.svg)

Open Inventor
=============

Open Inventor is an object oriented scene graph library implemented in C++
layered on top of OpenGL. It was originally developed by
[SGI](http://www.sgi.com/).

This Repository
===============

The aim of this repository is to integrate patches applied by various Linux
distributions and to apply build fixes for macOS.

It is based on an import of SGI's CVS repository at :pserver:cvs@oss.sgi.com:/cvs.
Currently, patches from Fedora and Debian are included. It also includes fixes for
font rendering on 64 bit Linux platforms.

A [CMake](https://cmake.org) build system has been added. It can be used
instead of traditional Makefiles.

For building on macOS, there is a [Homebrew](https://brew.sh)
[formula](https://github.com/hlrs-vis/homebrew-tap) based on this repository.

More Information
================

Refer to [SGI's Open Inventor page](http://oss.sgi.com/projects/inventor/)
([archive link](https://web.archive.org/web/20170811183842/http://oss.sgi.com/projects/inventor/)).
for more information and to the included README.FIRST for build instructions.
