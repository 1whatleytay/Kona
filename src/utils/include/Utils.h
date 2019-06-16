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

    class GenericException : public std::exception {
        std::string name;
        std::string message;
    public:
        const char *what() const noexcept override;

        GenericException(std::string name, std::string message);
    };

    std::string trim(const std::string &text);
    std::vector<std::string> split(const std::string &text, char delim);

    void log(const std::string& text);
}

#endif //KONA_UTILS_H
