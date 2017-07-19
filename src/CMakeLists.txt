

file(GLOB_RECURSE serial-filler_SRC
        "*.cpp"
        )

file(GLOB_RECURSE serial-filler_HEADERS
        "*.hpp")

add_library(SerialFiller ${serial-filler_SRC} ${serial-filler_HEADERS})
add_dependencies(SerialFiller HearYeHearYe)

target_include_directories(SerialFiller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# On Linux, "sudo make install" will typically copy the library libSerialFiller.a
# into the folder /usr/local/bin
install(TARGETS SerialFiller DESTINATION lib)

# On Linux, "sudo make install" will typically copy the SerialFiller
# folder into /usr/local/include
install(DIRECTORY include/SerialFiller DESTINATION include)