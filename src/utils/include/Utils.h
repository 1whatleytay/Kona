#ifndef KONA_UTILS_H
#define KONA_UTILS_H

#include <string>
#include <vector>

namespace Utils {
    class Arguments {
        std::vector<std::string> arguments;
    public:
        std::string operator[](int index) const;

        Arguments(int count, char **args);
    };

    void log(const std::string& text);
}

#endif //KONA_UTILS_H
