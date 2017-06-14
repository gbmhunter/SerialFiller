

file(GLOB_RECURSE serial-filler_SRC
        "*.cpp"
        "*.hpp"
        )

add_library (serial-filler ${serial-filler_SRC})

target_include_directories (serial-filler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})