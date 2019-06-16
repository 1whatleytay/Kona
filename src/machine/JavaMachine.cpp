#include <JavaMachine.h>
#include <JavaEnvironment.h>

#include <iostream>

typedef JavaEnvironmentMachineContext Context;

char *newString(const char *old) {
    char *newValue = new char[strlen(old)];
    strcpy(newValue, old);

    return newValue;
}

int popInt(Context context) {
    return context.frame.stack.popValue().valueInt;
}

template <typename T>
T *popRef(Context context) {
    return (T *)context.frame.stack.popValue().valueRef;
}

JavaEnvironmentInstance *popInst(Context context) {
    return (JavaEnvironmentInstance *)context.frame.stack.popValue().valueRef;
}

JavaEnvironmentMachineClass::JavaEnvironmentMachineClass(std::string name) : name(std::move(name)) {}

JavaFieldDescriptor &JavaEnvironmentMachineClass::getField(const JavaIdentifier &identifier) {
    for (JavaFieldDescriptor &field : fields) {
        if (identifier.matches(field)) {
            return field;
        }
    }

    throw Utils::GenericException("MissingMachineField",
        "Machine class " + name + " is missing field " + identifier.toString());
}

const JavaEnvironmentMachineMethod &JavaEnvironmentMachineClass::getMethod(const JavaIdentifier &identifier) {
    for (const JavaEnvironmentMachineMethod &method : methods) {
        if (identifier.matches(method.identifier)) {
            return method;
        }
    }

    throw Utils::GenericException("MissingMachineMethod",
        "Machine class " + name + " is missing method " + identifier.toString());
}

namespace Java {
    namespace Util {
        namespace Scanner {
            void constructor(Context context) {
                popInst(context);
                popInst(context);
            }

            void nextLine(Context context) {
                popInst(context);

                std::string out;
                std::cin >> out;

                context.frame.stack.push({.valueRef = newString(out.c_str())});
            }

            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/util/Scanner");
                thisClass.methods = {
                    {JavaIdentifier("<init>", "(Ljava/io/InputStream;)V"), constructor},
                    {JavaIdentifier("nextLine", "()Ljava/lang/String;"), nextLine},
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }
    }

    namespace Io {
        namespace InputStream {
            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/io/InputStream");

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }

        namespace PrintStream {
            void printlnString(Context context) {
                const char *text = popRef<const char>(context);
                popInst(context);

                std::cout << text << std::endl;
            }

            void printlnInt(Context context) {
                JavaInt value = popInt(context);
                popInst(context);

                std::cout << value << std::endl;
            }

            void println(Context context) {
                popInst(context);

                std::cout << std::endl;
            }

            void printString(Context context) {
                const char *text = popRef<const char>(context);
                popInst(context);

                std::cout << text;
            }

            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/io/PrintStream");
                thisClass.methods = {
                    {JavaIdentifier("println", "(Ljava/lang/String;)V"), printlnString},
                    {JavaIdentifier("println", "(I)V"), printlnInt},
                    {JavaIdentifier("println", "()V"), println},
                    {JavaIdentifier("print", "(Ljava/lang/String;)V"), printString},
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }
    }

    namespace Lang {
        namespace Object {
            void construct(Context context) {
                popInst(context);
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

        namespace String {
            void equals(Context context) {
                const char *a = popRef<const char>(context);
                const char *b = popRef<const char>(context);

                context.frame.stack.push({.valueInt=(strcmp(a, b) == 0)});
            }

            JavaEnvironmentMachineClass createClass() {
                JavaEnvironmentMachineClass thisClass = JavaEnvironmentMachineClass("java/lang/String");
                thisClass.methods = {
                    {JavaIdentifier("equals", "(Ljava/lang/Object;)Z"), equals}
                };

                return thisClass;
            }

            JavaEnvironmentMachineClass thisClass = createClass();
        }

        namespace StringBuilder {
            void construct(Context context) {
                popInst(context);
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
                    JavaFieldDescriptor("in", "Ljava/io/InputStream;", true,
                        {.valueRef = new JavaEnvironmentInstance(Java::Io::InputStream::thisClass)}),
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
        Java::Util::Scanner::thisClass,

        Java::Io::InputStream::thisClass,
        Java::Io::PrintStream::thisClass,

        Java::Lang::Object::thisClass,
        Java::Lang::String::thisClass,
        Java::Lang::StringBuilder::thisClass,
        Java::Lang::System::thisClass,
        Java::Lang::Math::thisClass,
    };
}
