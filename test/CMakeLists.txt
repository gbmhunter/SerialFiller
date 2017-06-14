

add_executable (serial-filler-tests main.cpp SerialFillerTests.cpp)


target_link_libraries (serial-filler-tests LINK_PUBLIC serial-filler gtest)