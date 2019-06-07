add_library(environment STATIC
        include/JavaEnvironment.h

        JavaEnvironment.cpp)

target_include_directories(environment PUBLIC include)
target_link_libraries(environment Kophi utils machine)
