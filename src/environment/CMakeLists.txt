add_library(environment STATIC
        include/JavaJar.h
        include/JavaEnvironment.h

        JavaJar.cpp
        JavaEnvironment.cpp)

target_include_directories(environment PUBLIC include)
target_link_libraries(environment zip Kophi utils machine)
