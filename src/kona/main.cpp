#include <Utils.h>

#include <Kophi/JavaClass.h>

#include <JavaEnvironment.h>

using namespace Kophi;

void list(const Utils::Arguments &arguments) {
    JavaClass java = JavaClass(arguments[1]);

    Utils::log(describeClass(java));
}

void run(const Utils::Arguments &arguments) {
    JavaClass java = JavaClass(arguments[1]);

    JavaEnvironment environment = JavaEnvironment(java);
    environment.run();
}

int main(int count, char **args) {
    Utils::Arguments arguments = Utils::Arguments(count, args);

    if (arguments[0] == "list") list(arguments);
    else if (arguments[0] == "run") run(arguments);
}
