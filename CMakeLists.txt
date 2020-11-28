cmake_minimum_required(VERSION 3.1)
set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_STANDARD 98)
set (CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


project(OpenInventor VERSION 2.1.5.10 LANGUAGES CXX C)

option(USE_INTERNAL_GLW "Use included copy of GLw (OpenGL Xt/Motif widgets) library" ON)
option(INSTALL_RPATH "Apply rpath when installing" ON)


SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
# this is used to override some of cmake's original find-modules
IF(APPLE)
    # don't use Frameworks and use X11 with matching OpenGL
    set(CMAKE_FIND_FRAMEWORK NEVER)
    SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/apple;${CMAKE_MODULE_PATH}")
ENDIF()

set(BUILD_SHARED_LIBS TRUE)
if (BUILD_SHARED_LIBS AND INSTALL_RPATH)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

find_program(SED_EXECUTABLE
    NAMES sed
    DOC "sed stream editor"
    REQUIRED
)

find_package(BISON REQUIRED)
find_package(StandardMathLibrary REQUIRED)
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

include(GNUInstallDirs)
add_dirvar(IVBINDIR "${CMAKE_INSTALL_BINDIR}")
add_dirvar(IVHELPDIR "${CMAKE_INSTALL_DATADIR}/inventor/help")
add_dirvar(IVFONTPATH "${CMAKE_INSTALL_DATADIR}/inventor/fonts")
set(IVDATAROOT "${CMAKE_INSTALL_DATADIR}/inventor/data")
add_dirvar(IVDATADIR "${IVDATAROOT}/models")
add_dirvar(IVMATERIALSDIR "${IVDATAROOT}/materials")
add_dirvar(IVTEXTURESDIR "${IVDATAROOT}/textures")
add_dirvar(IVDEMODATADIR "${IVDATAROOT}/demos")
#add_dirvar(IVDEMOBINDIR "libexec/inventor/demos")
#add_dirvar(IVDEMOBINDIR "lib/inventor/demos")
add_dirvar(IVDEMOBINDIR "lib/inventor")
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
