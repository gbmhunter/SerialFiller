

file(GLOB_RECURSE serial-filler_SRC
        "*.cpp"
        )

file(GLOB_RECURSE serial-filler_HEADERS
        "*.hpp")

add_library (serial-filler ${serial-filler_SRC} ${serial-filler_HEADERS})

target_include_directories (serial-filler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS serial-filler DESTINATION lib)
install(DIRECTORY include/SerialFiller DESTINATION include)