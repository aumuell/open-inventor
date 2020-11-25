# Find GLw - OpenGL Xt/Motif widgets library
#
# This module defines
#  GLw_FOUND - whether the snappy library was found
#  GLw_LIBRARIES - the snappy library
#  GLw_INCLUDE_DIR - the include path of the snappy library
#

find_library(GLw_LIBRARIES
    NAMES
    GLw
    PATHS
)

find_path(GLw_INCLUDE_DIR
    NAMES
    GL/GLwDrawA.h
    PATHS
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLw DEFAULT_MSG GLw_LIBRARIES GLw_INCLUDE_DIR)

if (GLw_FOUND)
    set(GLw_INCLUDE_DIRS ${GLw_INCLUDE_DIR})
endif()
