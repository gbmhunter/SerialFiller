
#file(GLOB_RECURSE SerialFillerExample_SRC
#        "*.cpp"
#        "*.hpp"
#        )

add_executable(BasicExample BasicExample.cpp)
target_link_libraries(BasicExample LINK_PUBLIC SerialFiller)

add_executable(ExceptionCheckingExample ExceptionCheckingExample.cpp)
target_link_libraries(ExceptionCheckingExample LINK_PUBLIC SerialFiller)