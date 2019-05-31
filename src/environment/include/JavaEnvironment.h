#ifndef KONA_JAVAENVIRONMENT_H
#define KONA_JAVAENVIRONMENT_H

#include <Utils.h>
#include <JavaUtils.h>

#include <Kophi/JavaClass.h>
#include <Kophi/JavaInstruction.h>

#include <stack>
#include <unordered_map>

#include <JavaMachine.h>

using namespace Kophi;

const JavaIdentifier mainId = JavaIdentifier("main", "([Ljava/lang/String;)V");

class JavaEnvironmentClass : public JavaClass {
public:
    const JavaMethod &getMethod(const JavaIdentifier &id);

    explicit JavaEnvironmentClass(const JavaClass &java);
};

class JavaEnvironmentInstance {
public:
    std::unordered_map<std::string, JavaTypeAny> fields;

    explicit JavaEnvironmentInstance(JavaEnvironmentClass &type, bool isStatic = false);

    explicit JavaEnvironmentInstance(JavaEnvironmentMachineClass &type);
};

class JavaEnvironmentStack : public std::stack<JavaTypeAny> {
public:
    JavaTypeAny popValue();

    void duplicate();
};

class JavaEnvironmentFrame {
public:
    std::vector<JavaTypeAny> locals;
    JavaEnvironmentStack stack;

    std::string debugStack();

    JavaEnvironmentFrame(unsigned localSize, unsigned stackSize);

    JavaEnvironmentFrame(unsigned localSize, unsigned stackSize,
                         JavaEnvironmentFrame &lastFrame, const std::string &descriptor, bool refernce);
};

class JavaEnvironment {
    std::vector<JavaEnvironmentClass> classes;
    JavaEnvironmentClass &main;

    std::vector<JavaEnvironmentMachineClass> standard;

    JavaEnvironmentClass *getClass(const std::string &name);

    JavaEnvironmentMachineClass *getStandardClass(const std::string &name);

    JavaReturnType execute(const JavaMethod &method, const JavaAttributeCode *code, JavaEnvironmentFrame &frame);

    JavaReturnType run(const JavaMethod &method, JavaEnvironmentFrame &lastFrame, bool reference);

    void run(const JavaMethod &method);

public:
    void run();

    JavaEnvironmentClass &loadClass(const JavaClass &java);

    explicit JavaEnvironment(const JavaClass &java);
};

#endif //KONA_JAVAENVIRONMENT_H