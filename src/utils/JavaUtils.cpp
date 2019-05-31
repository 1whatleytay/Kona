#include <utility>

#include <JavaUtils.h>

bool JavaIdentifier::matches(const JavaIdentifier &identifier) const {
    return identifier.name == name && identifier.descriptor == descriptor;
}

bool JavaIdentifier::matches(const JavaProperty &property) const {
    return property.getName() == name && property.getDescriptor() == descriptor;
}

JavaIdentifier::JavaIdentifier(std::string name, std::string descriptor)
    : name(std::move(name)), descriptor(std::move(descriptor)) {}

JavaIdentifier::JavaIdentifier(const JavaConstantNameAndType *nameAndType)
    : name(nameAndType->getName()), descriptor(nameAndType->getDescriptor()) {}

JavaFieldDescriptor::JavaFieldDescriptor(std::string name, std::string descriptor,
                                         bool isStatic, JavaTypeAny value)
                                         : JavaIdentifier(std::move(name), std::move(descriptor)),
                                         isStatic(isStatic), value(value) {}
