#include <Utils.h>

#include <sstream>
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

    const char* GenericException::what() const noexcept {
        return (name + ": " + message).c_str();
    }

    GenericException::GenericException(std::string name, std::string message)
        : name(std::move(name)), message(std::move(message)) { }

    void trimLeft(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](int ch) { return !std::isspace(ch) && (ch != '\r'); }));
    }

    void trimRight(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            [](int ch) { return !std::isspace(ch) && (ch != '\r'); }).base(), s.end());
    }

    std::string trim(const std::string &text) {
        std::string out = text;
        trimLeft(out);
        trimRight(out);
        return out;
    }

    std::vector<std::string> split(const std::string &text, char delim) {
        std::vector<std::string> result;
        std::stringstream stream = std::stringstream(text);

        std::string temp;
        while (getline(stream, temp, delim)) {
            if (Utils::trim(temp).empty()) continue;
            result.push_back(temp);
        }

        return result;
    }

    void log(const std::string &text) {
        std::cout << text << std::endl;
    }
}
