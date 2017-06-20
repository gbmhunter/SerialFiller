
file(GLOB_RECURSE SerialFillerExample_SRC
        "*.cpp"
        "*.hpp"
        )

add_executable (SerialFillerExample ${SerialFillerExample_SRC})

target_link_libraries (SerialFillerExample LINK_PUBLIC SerialFiller)