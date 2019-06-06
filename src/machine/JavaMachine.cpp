#include <JavaMachine.h>
#include <JavaEnvironment.h>

typedef JavaEnvironmentMachineContext Context;

JavaEnvironmentMachineClass::JavaEnvironmentMachineClass(std::string name) : name(std::move(name)) {}

JavaFieldDescriptor &JavaEnvironmentMachineClass::getField(const JavaIdentifier &identifier) {
    for (JavaFieldDescriptor &field : fields) {
        if (identifier.matches(field)) {
            return field;
        }
    }

    throw std::exception();
}

const JavaEnvironmentMachineMethod &JavaEnvironmentMachineClass::getMethod(const JavaIdentifier &identifier) {
    for (const JavaEnvironmentMachineMethod &method : methods) {
        if (identifier.matches(method.identifier)) {
            return method;
        }
    }

    throw std::exception();
}

namespace Java {
    namespace Io {
        namespace PrintStream {
            void printlnString(Context context) {
                const char *text = (const char *) context.frame.stack.popValue().valueRef;
                JavaEnvironmentInstance *instance = (JavaEnvironmentInstance *) context.frame.stack.popValue().valueRef;

                Utils::log(std::string(text));
            }

            void printlnInt(Context context) {
                JavaInt value = context.frame.stack.popValue().valueInt;
                JavaEnvironmentInstance *instance = (JavaEnvironmentInstance *) context.frame.stack.popValue().valueRef;

                Utils::log(std::to_string(value));
            }

            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/io/PrintStream");
                thisClass.methods = {
                    {JavaIdentifier("println", "(Ljava/lang/String;)V"), printlnString},
                    {JavaIdentifier("println", "(I)V"), printlnInt}
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }
    }

    namespace Lang {
        namespace Object {
            void construct(Context context) {

            }

            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/lang/Object");
                thisClass.methods = {
                    {JavaIdentifier("<init>", "()V"), construct}
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }

        namespace StringBuilder {
            void construct(Context context) {
                context.frame.stack.pop();
            }

            void appendString(Context context) {
                const char *appendText = (const char *) context.frame.stack.popValue().valueRef;
                JavaEnvironmentInstance *instance = (JavaEnvironmentInstance *) context.frame.stack.popValue().valueRef;
                const char *text = (const char *) instance->fields["text"].valueRef;

                unsigned size = strlen(text) + strlen(appendText) + 1;
                char *newText = new char[size];
                std::memcpy(newText, text, strlen(text));
                std::memcpy(&newText[strlen(text)], appendText, strlen(appendText));
                newText[size - 1] = '\0';

                instance->fields["text"].valueRef = newText;

                context.frame.stack.push({.valueRef = instance});
            }

            void appendInt(Context context) {
                std::string appendText = std::to_string(context.frame.stack.popValue().valueInt);
                JavaEnvironmentInstance *instance = (JavaEnvironmentInstance *) context.frame.stack.popValue().valueRef;
                const char *text = (const char *) instance->fields["text"].valueRef;

                unsigned size = strlen(text) + appendText.size() + 1;
                char *newText = new char[size];
                std::memcpy(newText, text, strlen(text));
                std::memcpy(&newText[strlen(text)], appendText.c_str(), appendText.size());
                newText[size - 1] = '\0';

                instance->fields["text"].valueRef = newText;

                context.frame.stack.push({.valueRef = instance});
            }

            void toString(Context context) {
                JavaEnvironmentInstance *instance = (JavaEnvironmentInstance *) context.frame.stack.popValue().valueRef;

                context.frame.stack.push(instance->fields["text"]);
            }

            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/lang/StringBuilder");
                thisClass.methods = {
                    {JavaIdentifier("<init>", "()V"),                                           construct},
                    {JavaIdentifier("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;"), appendString},
                    {JavaIdentifier("append", "(I)Ljava/lang/StringBuilder;"),                  appendInt},
                    {JavaIdentifier("toString", "()Ljava/lang/String;"),                        toString},
                };
                thisClass.fields = {
                    JavaFieldDescriptor("text", "Ljava/lang/String;", false, {.valueRef = (void *) ""}),
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }

        namespace System {
            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/lang/System");
                thisClass.fields = {
                    JavaFieldDescriptor("out", "Ljava/io/PrintStream;", true,
                                        {.valueRef = new JavaEnvironmentInstance(Java::Io::PrintStream::thisClass)}),
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }

        namespace Math {
            void max(Context context) {
                context.frame.stack.push(
                    {.valueInt = std::max(
                        context.frame.stack.popValue().valueInt,
                        context.frame.stack.popValue().valueInt)});
            }

            void min(Context context) {
                context.frame.stack.push(
                    {.valueInt = std::min(
                        context.frame.stack.popValue().valueInt,
                        context.frame.stack.popValue().valueInt)});
            }

            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/lang/Math");
                thisClass.methods = {
                    {JavaIdentifier("max", "(II)I"), Java::Lang::Math::max},
                    {JavaIdentifier("min", "(II)I"), Java::Lang::Math::min},
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }
    }
}

std::vector<JavaEnvironmentMachineClass> getStandard() {
    return {
        Java::Io::PrintStream::thisClass,

        Java::Lang::Object::thisClass,
        Java::Lang::StringBuilder::thisClass,
        Java::Lang::System::thisClass,
        Java::Lang::Math::thisClass,
    };
}
