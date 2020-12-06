cmake_minimum_required(VERSION 3.14)
set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_STANDARD 98)
set (CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


project(OpenInventor VERSION 2.1.5.10 LANGUAGES CXX C)

include(GNUInstallDirs)
include(FeatureSummary)

option(INVENTOR_INTERNAL_GLW "Use included copy of GLw (OpenGL Xt/Motif widgets) library" ON)
option(INVENTOR_INSTALL_RPATH "Apply rpath when installing" ON)
option(INVENTOR_SANITIZE_UNDEFINED "Build with -fsanitize=undefined" OFF)
option(INVENTOR_SANITIZE_ADDRESS "Build with -fsanitize=address" OFF)
option(INVENTOR_SANITIZE_THREAD "Build with -fsanitize=thread" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
# this is used to override some of cmake's original find-modules
if(APPLE)
    # don't use Frameworks and use X11 with matching OpenGL
    set(CMAKE_FIND_FRAMEWORK NEVER)
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/apple;${CMAKE_MODULE_PATH}")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "")
   set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
      "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
      FORCE
   )
   message("Setting CMAKE_BUILD_TYPE to ${CMAKE_BUILD_TYPE}")
endif()

set(BUILD_SHARED_LIBS TRUE)
if (BUILD_SHARED_LIBS AND INVENTOR_INSTALL_RPATH)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-write-strings)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-c++11-compat-deprecated-writable-strings)
    add_compile_options(-Wno-backslash-newline-escape)
    add_compile_options(-Wno-dangling-else)
    add_compile_options(-Wno-parentheses)
    add_compile_options(-Wno-switch)
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fno-strict-aliasing)
    add_compile_options(-funsigned-char)
endif()

set(ASAN_COMPILE_FLAGS)
set(ASAN_LINK_FLAGS)
if (INVENTOR_SANITIZE_UNDEFINED)
   set(ASAN_LINK_FLAGS ${ASAN_LINK_FLAGS} -fsanitize=undefined)
   set(ASAN_COMPILE_FLAGS ${ASAN_COMPILE_FLAGS} -fsanitize=undefined)
endif()
if (INVENTOR_SANITIZE_ADDRESS)
   set(ASAN_LINK_FLAGS ${ASAN_LINK_FLAGS} -fsanitize=address)
   set(ASAN_COMPILE_FLAGS ${ASAN_COMPILE_FLAGS} -fsanitize=address)
   set(ASAN_COMPILE_FLAGS${ASAN_COMPILE_FLAGS} -fsanitize-address-use-after-scope)
endif()
if (INVENTOR_SANITIZE_THREAD)
   set(ASAN_LINK_FLAGS ${ASAN_LINK_FLAGS} -fsanitize=thread)
   set(ASAN_COMPILE_FLAGS ${ASAN_COMPILE_FLAGS} -fsanitize=thread)
endif()
if (INVENTOR_SANITIZE_THREAD OR INVENTOR_SANITIZE_ADDRESS OR INVENTOR_SANITIZE_UNDEFINED)
   set(ASAN_COMPILE_FLAGS ${ASAN_COMPILE_FLAGS} -fno-omit-frame-pointer)
   if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
       add_compile_options(${ASAN_COMPILE_FLAGS})
       add_link_options(${ASAN_LINK_FLAGS})
   endif()
endif()

find_program(SED_EXECUTABLE
    NAMES sed
    DOC "sed stream editor"
    REQUIRED
)

find_package(StandardMathLibrary)
find_package(BISON REQUIRED)
find_package(JPEG REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(Iconv REQUIRED)
find_package(Motif REQUIRED)
if (NOT USE_INTERNAL_GLW)
    find_package(GLw REQUIRED)
endif()

macro(ADD_DIRVAR VAR DIR)
    set(${VAR} ${DIR})
    add_definitions(-D${VAR}="${CMAKE_INSTALL_PREFIX}/${DIR}")
endmacro()

add_dirvar(IVBINDIR "${CMAKE_INSTALL_BINDIR}")
add_dirvar(IVHELPDIR "${CMAKE_INSTALL_DATADIR}/inventor/help")
add_dirvar(IVFONTPATH "${CMAKE_INSTALL_DATADIR}/inventor/fonts")
set(IVDATAROOT "${CMAKE_INSTALL_DATADIR}/inventor/data")
add_dirvar(IVDATADIR "${IVDATAROOT}/models")
add_dirvar(IVMATERIALSDIR "${IVDATAROOT}/materials")
add_dirvar(IVTEXTURESDIR "${IVDATAROOT}/textures")
add_dirvar(IVDEMODATADIR "${IVDATAROOT}/demos")
#add_dirvar(IVDEMOBINDIR "libexec/inventor")
add_dirvar(IVDEMOBINDIR "${CMAKE_INSTALL_LIBDIR}/inventor")

set(IVHDRTOP Inventor)

if(APPLE)
    set(PDFVIEWER "open")
else()
    set(PDFVIEWER "xdg-open")
endif()
add_definitions(-DPDFVIEWER="${PDFVIEWER}")

add_subdirectory(tools)
add_subdirectory(libimage)
add_subdirectory(libFL/ang)
add_subdirectory(lib)
add_subdirectory(libSoXt)
add_subdirectory(doc)
add_subdirectory(apps)
add_subdirectory(data)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)
