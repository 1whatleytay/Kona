#include <Utils.h>

#include <Kophi/JavaClass.h>

#include <JavaJar.h>
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

void jar(const Utils::Arguments &arguments) {
    JavaJar jar = JavaJar(arguments[1]);

    JavaEnvironment environment = JavaEnvironment(jar);
    environment.run();
}

int main(int count, char **args) {
    Utils::Arguments arguments = Utils::Arguments(count, args);

    if (arguments[0] == "list") list(arguments);
    else if (arguments[0] == "run") run(arguments);
    else if (arguments[0] == "jar") jar(arguments);
    else Utils::log("No command for " + arguments[0]);

    return 0;
}
