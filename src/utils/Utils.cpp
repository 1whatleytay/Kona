#include <Utils.h>

#include <iostream>

namespace Utils {
    std::string Arguments::operator[](int index) const {
        index++;
        if (index >= arguments.size()) throw std::exception();
        return arguments[index];
    }

    Arguments::Arguments(int count, char **args) {
        arguments.reserve(count);
        for (int a = 0; a < count; a++) {
            arguments.emplace_back(args[a]);
        }
    }

    void log(const std::string &text) {
        std::cout << text << std::endl;
    }
}
