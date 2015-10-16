cmake_minimum_required(VERSION 2.8.3)

project(USBHost)
set(EXE_NAME host)

#####################################
## Add C++ 11 Flags
#####################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wdouble-promotion -Wformat=2 -Wuninitialized -Wsuggest-attribute=noreturn -Wtrampolines -pipe")

# Compiler-specific C++11 activation.
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater for c++11 features.")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(libusb-1.0 REQUIRED)

include_directories(${LIBUSB_1_INCLUDE_DIRS})

find_package(Threads)


#####################################
## Create main program
#####################################

# Add custom definitions
add_definitions(
    -DAPPLICATION_NAME="${PROJECT_NAME}"
    -DAPPLICATION_EXE_NAME="${EXE_NAME}"
    -DPLUGINS_DIR="${PLUGINS_DIR}"
    # Defines some macro for EasyLogging++
    -DELPP_UNICODE
    -DELPP_THREAD_SAFE
    -DELPP_FORCE_USE_STD_THREAD
    -DELPP_STACKTRACE_ON_CRASH
    -DELPP_WINSOCK2
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
)

# Set executable output
add_executable(
    ${EXE_NAME}
    ${source_files}
)

target_link_libraries(
    ${EXE_NAME}
    ${LIBUSB_1_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
