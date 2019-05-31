add_library(utils
        include/Utils.h
        include/JavaUtils.h

        Utils.cpp
        JavaUtils.cpp)

target_include_directories(utils PUBLIC include)
target_link_libraries(utils Kophi)
