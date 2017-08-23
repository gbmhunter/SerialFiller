cmake_minimum_required(VERSION 2.8.11)
project(SerialFiller)

add_definitions(-std=c++11)

option(BUILD_DEPENDENCIES "If set to ON, dependencies will be downloaded and built as part of the build process." ON)

if (BUILD_DEPENDENCIES)
    message("BUILD_DEPENDENCIES=ON, dependencies will be downloaded and built automatically.")
    # EXTERNAL_INSTALL_LOCATION is used by external projects, except for gtest
    include(ExternalProject)
    set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
else ()
    message("BUILD_DEPENDENCIES=OFF, dependencies have to be downloaded, built and installed manually.")
endif ()

#=================================================================================================#
#============================================== gtest ============================================#
#=================================================================================================#

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
# EXCLUDE_FROM_ALL prevents gtest and gmock from being installed
# when make install is called
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
        "${CMAKE_BINARY_DIR}/googletest-build" EXCLUDE_FROM_ALL)

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
            "${gmock_SOURCE_DIR}/include")
endif ()

#=================================================================================================#
#========================================= This Project ==========================================#
#=================================================================================================#

# Now simply link your own targets against gtest, gmock,
# etc. as appropriate
include_directories(include)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
