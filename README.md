[![make build status](https://github.com/aumuell/open-inventor/workflows/make/badge.svg)](https://github.com/aumuell/open-inventor/actions?query=workflow%3Amake)
[![CMake build status](https://github.com/aumuell/open-inventor/workflows/CMake/badge.svg)](https://github.com/aumuell/open-inventor/actions?query=workflow%3ACMake)
[![macOS build status](https://github.com/aumuell/open-inventor/workflows/macOS/badge.svg)](https://github.com/aumuell/open-inventor/actions?query=workflow%3AmacOS)

Open Inventor
=============

Open Inventor is an object oriented scene graph library implemented in C++
layered on top of OpenGL. It was originally developed by
[SGI](http://www.sgi.com/).

This Repository
===============

The aim of this repository is to integrate patches applied by various Linux
distributions and to apply build fixes for macOS.
It is based on an import of SGI's CVS repository at `:pserver:cvs@oss.sgi.com:/cvs`.

Currently, patches from Fedora and Debian are included. It also includes bug fixes,
most notably for font rendering on 64 bit Linux platforms.
A [CMake](https://cmake.org) build system has been added. It can be used
instead of the traditional Makefiles.
The precompiled font library `libFL_i386.a` and unused RPM .spec files have been removed.

Building and Installation
=========================

You can build with CMake like this:

    git clone https://github.com/aumuell/open-inventor
    mkdir open-inventor-build
    cd open-inventor-build
    cmake -DCMAKE_INSTALL_PREFIX=/opt/inventor ../open-inventor
    make -j10
    make install

The last step is optional, as all the programs that do not try to read from
hard-coded paths also work from the build directory.

The included `README.FIRST` has instructions on how to use the original make
build system.

For building on macOS, there is a [Homebrew](https://brew.sh)
[formula](https://github.com/hlrs-vis/homebrew-tap) based on this repository.

More Information
================

Refer to [SGI's Open Inventor page](http://oss.sgi.com/projects/inventor/)
([archive link](https://web.archive.org/web/20170811183842/http://oss.sgi.com/projects/inventor/))
for more information.

There are also helpful books for studying Open Inventor:

- **The Inventor Mentor**: Programming Object-Oriented 3D Graphics With Open Inventor, Release 2
  ([html](http://www-evasion.imag.fr/Membres/Francois.Faure/doc/inventorMentor/sgi_html/),
   [pdf](http://www.cs.ualberta.ca/~graphics/books/mentor.pdf))  
  *Josie Wernecke*  
  Addison-Wesley, 1994  
  ISBN: 978-0-201-62495-3  

  This book illustrates how to use Open Inventor.

- **The Inventor Toolmaker**: Extending Open Inventor, Release 2
    ([html](http://www-evasion.imag.fr/Membres/Francois.Faure/doc/inventorToolmaker/sgi_html/index.html),
     [pdf](http://www.cs.ualberta.ca/~graphics/books/toolmaker.pdf))  
  *Josie Wernecke, Open Inventor Architecture Group*  
  Addison-Wesley, 1994  
  ISBN: 978-0-201-62493-9  

  This book covers how to extend the functionality of Open Inventor.
