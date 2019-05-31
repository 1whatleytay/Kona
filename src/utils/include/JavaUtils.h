#ifndef KONA_JAVAUTILS_H
#define KONA_JAVAUTILS_H

#include <Kophi/JavaClass.h>

using namespace Kophi;

typedef void *JavaRef;

union JavaTypeAny {
    JavaInt valueInt;
    JavaLong valueLong;
    JavaFloat valueFloat;
    JavaDouble valueDouble;
    JavaRef valueRef;
};

enum class JavaType {
    Void,
    Boolean,
    Byte,
    Char,
    Short,
    Int,
    Float,
    Ref,
    Long,
    Double,
};

class JavaReturnType {
public:
    JavaType type;
    JavaTypeAny value;
};

class JavaIdentifier {
public:
    std::string name;
    std::string descriptor;

    bool matches(const JavaIdentifier &identifier) const;
    bool matches(const JavaProperty &property) const;

    JavaIdentifier(std::string name, std::string descriptor);
    explicit JavaIdentifier(const JavaConstantNameAndType *nameAndType);
};

class JavaFieldDescriptor : public JavaIdentifier {
public:
    bool isStatic;
    JavaTypeAny value;

    JavaFieldDescriptor(std::string name, std::string descriptor, bool isStatic, JavaTypeAny value = { });
};

#endif //KONA_JAVAUTILS_H
