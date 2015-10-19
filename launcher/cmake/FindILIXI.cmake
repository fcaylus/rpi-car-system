#
# CMake module to search for ilixi library
#
# If it's found it sets ILIXI_FOUND to TRUE
# and following variables are set:
#    ILIXI_INCLUDE_DIR
#    ILIXI_LIBRARY


# FIND_PATH and FIND_LIBRARY normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing.

FIND_PATH(ILIXI_INCLUDE_DIR ilixiGUI.h
  HINTS "$ENV{ILIXI_INCLUDE_PATH}"
  PATHS
    # Env
    "$ENV{LIB_DIR}/include"
    "$ENV{LIB_DIR}/include/ilixi-1.0.0"
    #
	"/opt/ilx/include"
	"/opt/ilx/include/ilixi-1.0.0"
    "/usr/include"
    "/usr/include/ilixi-1.0.0"
    "/usr/local/include"
    "/usr/local/include/ilixi-1.0.0"
  )
FIND_PATH(ILIXI_INCLUDE_DIR PATHS "${CMAKE_INCLUDE_PATH}/ilixi-1.0.0" NAMES ilixiGUI.h)


FIND_LIBRARY(ILIXI_LIBRARY NAMES ilixi-1.0.0 libilixi-1.0.0
  HINTS "$ENV{ILIXI_LIBRARY_PATH}"
  PATHS
	"/opt/ilx/lib"
    "$ENV{LIB_DIR}/lib"
  )
FIND_LIBRARY(ILIXI_LIBRARY NAMES ilixi-1.0.0 libilixi-1.0.0)

IF (ILIXI_INCLUDE_DIR AND ILIXI_LIBRARY)
   SET(ILIXI_FOUND TRUE)
ENDIF (ILIXI_INCLUDE_DIR AND ILIXI_LIBRARY)

IF (ILIXI_FOUND)
   get_filename_component(ILIXI_LIB_DIR ${ILIXI_LIBRARY} PATH)
   IF (NOT ILIXI_FIND_QUIETLY)
      MESSAGE(STATUS "Found ILIXI include-dir path: ${ILIXI_INCLUDE_DIR}")
      MESSAGE(STATUS "Found ILIXI library path: ${ILIXI_LIBRARY}")
   ENDIF (NOT ILIXI_FIND_QUIETLY)
ELSE (ILIXI_FOUND)
   IF (ILIXI_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find ILIXI")
   ENDIF (ILIXI_FIND_REQUIRED)
ENDIF (ILIXI_FOUND)

