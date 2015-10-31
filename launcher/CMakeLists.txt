cmake_minimum_required(VERSION 2.8.3)

project(Launcher)
set(EXE_NAME launcher)

#####################################
## Add C++ 11 Flags
#####################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wdouble-promotion -Wformat=2 -Wuninitialized -Wsuggest-attribute=noreturn -Wtrampolines -pipe -pedantic")

# Compiler-specific C++11 activation.
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater for c++11 features.")
    endif()
else()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif()

#####################################
## Set default build type
#####################################

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

message(STATUS "This is a ${CMAKE_BUILD_TYPE} build !")

include_directories(.)

#####################################
## Add dependencies
#####################################

include_directories(thirdparty)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/thirdparty/libvlcpp/cmake" "${CMAKE_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)

# Find LibXML2
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

# Add libvlc and libvlcpp
find_package(LIBVLC REQUIRED)
include_directories("${LIBVLC_INCLUDE_DIR}" "${CMAKE_SOURCE_DIR}/thirdparty/libvlcpp")
file(GLOB LIBVLCPP_HEADERS "${CMAKE_SOURCE_DIR}/thirdparty/libvlcpp/vlcpp/*.hpp")

# Add ilixi
find_package(ILIXI REQUIRED)
include_directories("${ILIXI_INCLUDE_DIR}")

# Add DirectFB
find_package(DirectFB REQUIRED)
include_directories("${DIRECTFB_INCLUDE_DIR}")

# Add sigcpp
find_package(SIGC++ REQUIRED)
include_directories("${SIGC++_INCLUDE_DIR}")

# Add libusb
find_package(libusb-1.0 REQUIRED)
include_directories(${LIBUSB_1_INCLUDE_DIRS})


#####################################
## Check cross compilation for rpi-car-system
#####################################

# If the variable READY_FOR_CARSYSTEM, some path will be adjusted in the program to match the rpi-car-system configuration
if("${READY_FOR_CARSYSTEM}" STREQUAL "yes")
    add_definitions(-DREADY_FOR_CARSYSTEM)
else()
    add_definitions(-DELPP_STACKTRACE_ON_CRASH) # execinfo.h not available on arm
endif()

#####################################
## Create main program
#####################################

# Add custom definitions
add_definitions(
    -DAPPLICATION_NAME="${PROJECT_NAME}"
    -DAPPLICATION_EXE_NAME="${EXE_NAME}"
    # Defines some macro for EasyLogging++
    -DELPP_UNICODE
    -DELPP_THREAD_SAFE
    -DELPP_FORCE_USE_STD_THREAD
    -DELPP_STL_LOGGING
    -DELPP_LOG_STD_ARRAY
)

# Add specific debug definitions
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(
        -DELPP_DEBUG_ERRORS
        -DDEBUG_BUILD
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# Add src files
file(
    GLOB_RECURSE
    source_files
    src/*.cpp
    src/*.h
    src/widgets/*.h
    src/widgets/*.cpp
)

# Set executable output
add_executable(
    ${EXE_NAME}
    ${source_files}
    ${LIBVLCPP_HEADERS}
)

target_link_libraries(
    ${EXE_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LIBUSB_1_LIBRARIES}
    ${SIGC++_LIBRARY}
    ${LIBXML2_LIBRARIES}
    ${LIBVLCCORE_LIBRARY}
    ${LIBVLC_LIBRARY}
    ${ILIXI_LIBRARY}
)

# Copy "theme" files when build finish
add_custom_command(
    TARGET ${EXE_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/theme
    $<TARGET_FILE_DIR:${EXE_NAME}>/theme
    COMMENT "Copy theme directory ..."
)
