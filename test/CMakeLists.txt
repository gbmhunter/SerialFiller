
file(GLOB_RECURSE serial-filler-tests_SRC
        "*.cpp"
        "*.hpp"
        )

add_executable (serial-filler-tests ${serial-filler-tests_SRC})


target_link_libraries (serial-filler-tests LINK_PUBLIC SerialFiller gtest)