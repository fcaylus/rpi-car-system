#
# CMake module to search for directfb library
#
# If it's found it sets DIRECTFB_FOUND to TRUE
# and following variables are set:
#    DIRECTFB_INCLUDE_DIR
#    DIRECTFB_LIBRARY


# FIND_PATH and FIND_LIBRARY normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing.

FIND_PATH(DIRECTFB_INCLUDE_DIR directfb.h
  HINTS "$ENV{DIRECTFB_INCLUDE_PATH}"
  PATHS
    # Env
    "$ENV{LIB_DIR}/include"
    "$ENV{LIB_DIR}/include/directfb"
    #
	"/opt/ilx/include"
	"/opt/ilx/include/directfb"
    "/usr/include"
    "/usr/include/directfb"
    "/usr/local/include"
    "/usr/local/include/directfb"
  )
FIND_PATH(DIRECTFB_INCLUDE_DIR PATHS "${CMAKE_INCLUDE_PATH}/directfb" NAMES directfb.h)


FIND_LIBRARY(DIRECTFB_LIBRARY NAMES directfb libdirectfb
  HINTS "$ENV{DIRECTFB_LIBRARY_PATH}"
  PATHS
	"/opt/ilx/lib"
    "$ENV{LIB_DIR}/lib"
  )
FIND_LIBRARY(DIRECTFB_LIBRARY NAMES directfb libdirectfb)

IF (DIRECTFB_INCLUDE_DIR AND DIRECTFB_LIBRARY)
   SET(DIRECTFB_FOUND TRUE)
ENDIF (DIRECTFB_INCLUDE_DIR AND DIRECTFB_LIBRARY)

IF (DIRECTFB_FOUND)
   get_filename_component(DIRECTFB_LIB_DIR ${DIRECTFB_LIBRARY} PATH)
   IF (NOT DIRECTFB_FIND_QUIETLY)
      MESSAGE(STATUS "Found DIRECTFB include-dir path: ${DIRECTFB_INCLUDE_DIR}")
      MESSAGE(STATUS "Found DIRECTFB library path: ${DIRECTFB_LIBRARY}")
   ENDIF (NOT DIRECTFB_FIND_QUIETLY)
ELSE (DIRECTFB_FOUND)
   IF (DIRECTFB_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find DIRECTFB")
   ENDIF (DIRECTFB_FIND_REQUIRED)
ENDIF (DIRECTFB_FOUND)

