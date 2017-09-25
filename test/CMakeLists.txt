#
# \file 			CMakeLists.txt
# \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
# \edited           n/a
# \created			2017-06-10
# \last-modified	2017-09-25
# \brief 			Contains instructions for building the unit tests.
# \details
#		See README.md in root dir for more info.


file(GLOB_RECURSE SerialFillerTests_SRC
        "*.cpp"
        "*.hpp")

add_executable(SerialFillerTests ${SerialFillerTests_SRC})

target_link_libraries(SerialFillerTests LINK_PUBLIC SerialFiller gtest)

# The custom target and custom command below allow the unit tests
# to be run.
# If you want them to run automatically by CMake, uncomment #ALL
add_custom_target(
        run_unit_tests #ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/SerialFillerTests.touch SerialFillerTests)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/SerialFillerTests.touch
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/SerialFillerTests)