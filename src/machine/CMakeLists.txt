add_library(machine STATIC
        include/JavaMachine.h

        JavaMachine.cpp)

target_include_directories(machine PUBLIC include)
target_link_libraries(machine Kophi utils environment)
