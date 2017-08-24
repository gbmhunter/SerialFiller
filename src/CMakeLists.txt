

file(GLOB_RECURSE serial-filler_SRC
        "*.cpp")

file(GLOB_RECURSE serial-filler_HEADERS
        "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_library(SerialFiller ${serial-filler_SRC} ${serial-filler_HEADERS})

target_include_directories(SerialFiller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# On Linux, "sudo make install" will typically copy the library libSerialFiller.a
# into the folder /usr/local/bin
install(TARGETS SerialFiller DESTINATION lib)

# On Linux, "sudo make install" will typically copy the
# folder into /usr/local/include
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/SerialFiller DESTINATION include)