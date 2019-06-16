#ifndef KONA_JAVASTANDARD_H
#define KONA_JAVASTANDARD_H

#include <Kophi/Utils.h>

#include <Utils.h>
#include <JavaUtils.h>

#include <unordered_map>

using namespace Kophi;

class JavaEnvironment;
class JavaEnvironmentFrame;

class JavaEnvironmentMachineContext {
public:
    JavaEnvironment &environment;
    JavaEnvironmentFrame &frame;
};

typedef std::function<void(JavaEnvironmentMachineContext context)> JavaEnvironmentMachineFunction;

class JavaEnvironmentMachineMethod {
public:
    JavaIdentifier identifier;
    JavaEnvironmentMachineFunction function;
};

class JavaEnvironmentMachineClass {
public:
    std::string name;
    std::vector<JavaFieldDescriptor> fields;
    std::vector<JavaEnvironmentMachineMethod> methods;

    JavaFieldDescriptor &getField(const JavaIdentifier &identifier);

    const JavaEnvironmentMachineMethod &getMethod(const JavaIdentifier &identifier);

    explicit JavaEnvironmentMachineClass(std::string name);
};

std::vector<JavaEnvironmentMachineClass> getStandard();

#endif //KONA_JAVASTANDARD_H
