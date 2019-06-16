#include <JavaEnvironment.h>

#include <JavaJar.h>

#include <sstream>

const JavaMethod &JavaEnvironmentClass::getMethod(const JavaIdentifier &id) {
    for (const JavaMethod &method : java.methods) {
        if (id.matches(method)) {
            return method;
        }
    }

    throw std::exception();
}

JavaEnvironmentClass::JavaEnvironmentClass(const JavaClass &java) : java(java) {
    for (const JavaField &field : java.fields) {
        hasAccessFlag(field.)
    }
}

JavaEnvironmentInstance::JavaEnvironmentInstance(JavaEnvironmentClass &type, bool isStatic) {
    for (const JavaField &field : type.java.fields) {
        if (hasAccessFlag(field->accessFlags, AccessFlags::Static) == isStatic) {
            fields[field->getName()] = {0};

            const JavaAttribute &constantValue = field->searchAttributes("Constant");
            if (constantValue) {
                JavaConstant constant = JavaAttributeConstantValue::cast(constantValue)->getConstant();
                switch (constant->tag) {
                    default:
                        Utils::log("Unknown Tag: " + std::to_string((unsigned) constant->tag));
                        break;
                }
            }
        }
    }
}

JavaEnvironmentInstance::JavaEnvironmentInstance(JavaEnvironmentMachineClass &type) {
    for (const JavaFieldDescriptor &field : type.fields) {
        if (!field.isStatic) {
            fields[field.name] = field.value;
        }
    }
}

JavaEnvironmentArray::JavaEnvironmentArray(JavaArrayType type,
                                           JavaInt size) : type(type), size(size), arrayData(size) {}

JavaTypeAny JavaEnvironmentStack::popValue() {
    if (empty()) throw std::exception();
    JavaTypeAny result = top();
    pop();
    return result;
}

void JavaEnvironmentStack::duplicate() {
    push(top());
}

std::string JavaEnvironmentFrame::debugStack() {
    std::stringstream stream;
    JavaEnvironmentStack tempStack = stack;
    stream << "Stack Size: " << tempStack.size() << "\n";
    while (tempStack.size() > 0) {
        JavaTypeAny any = tempStack.popValue();
        stream << "\t" << "i: " << any.valueInt << " a: " << any.valueRef << "\n";
    }
    return stream.str();
}

JavaEnvironmentFrame::JavaEnvironmentFrame(unsigned localSize, unsigned stackSize) : locals(localSize) {}

JavaEnvironmentFrame::JavaEnvironmentFrame(unsigned localSize, unsigned stackSize, JavaEnvironmentFrame &lastFrame,
                                           const std::string &descriptor,
                                           bool reference) : JavaEnvironmentFrame(localSize, stackSize) {
    unsigned localIndex = reference ? 1 : 0;

    for (unsigned a = 0; a < descriptor.size(); a++) {
        switch (descriptor[a]) {
            case '(':
                continue;
            case ')':
                a = descriptor.size(); // Done Processing
                break;
            case 'J':
            case 'D':
                locals[localIndex].valueInt = lastFrame.stack.popValue().valueInt;
                localIndex++;
            case 'B':
            case 'C':
            case 'S':
            case 'I':
            case 'F':
                locals[localIndex].valueInt = lastFrame.stack.popValue().valueInt;
                break;
            case 'L':
                while (a < descriptor.size() && descriptor[a] != ';') { a++; }
                locals[localIndex].valueRef = lastFrame.stack.popValue().valueRef;
                break;
            case '[':
                while (a < descriptor.size() && descriptor[a] != '[') { a++; }
                if (descriptor[a] == 'L') {
                    while (a < descriptor.size() && descriptor[a] != ';') { a++; }
                }
                locals[localIndex].valueRef = lastFrame.stack.popValue().valueRef;
                break;
            default:
                throw std::exception();
        }
        localIndex++;
    }

    if (reference) {
        locals[0] = lastFrame.stack.popValue();
    }
}

JavaEnvironmentClass *JavaEnvironment::getClass(const std::string &name) {
    for (JavaEnvironmentClass &type : classes) {
        if (type.java.getThisClass()->getName() == name) {
            return &type;
        }
    }

    return nullptr;
}

JavaEnvironmentMachineClass *JavaEnvironment::getStandardClass(const std::string &name) {
    for (JavaEnvironmentMachineClass &java : standard) {
        if (java.name == name) {
            return &java;
        }
    }

    return nullptr;
}

JavaReturnType JavaEnvironment::execute(const JavaMethod &method,
                                        const JavaAttributeCode *code, JavaEnvironmentFrame &frame) {
    unsigned pc = 0;

    while (pc < code->codeLength) {
        JavaInstruction instruction = (JavaInstruction) code->code[pc];
        const JavaInstructionInfo &info = lookupInstruction(instruction);

        Utils::log(createDisasm(method->parent, &code->code[pc], info.length, pc));
        Utils::log(frame.debugStack());

        switch (instruction) {
            case JavaInstruction::Nop:
                break;

            case JavaInstruction::Ldc: {
                JavaConstant constant = method->parent.pool[code->code[pc + 1]];
                switch (constant->tag) {
                    case JavaConstantTag::String: {
                        JavaConstantString *text = JavaConstantString::cast(constant);
                        frame.stack.push({.valueRef = (char *) text->getText().c_str()});
                        break;
                    }
                    case JavaConstantTag::Integer: {
                        JavaConstantInteger *integer = JavaConstantInteger::cast(constant);
                        frame.stack.push({.valueInt = integer->value});
                        break;
                    }
                    default:
                        Utils::log("Unimplemented tag " + std::to_string((unsigned) constant->tag));
                        throw std::exception();
                }
                break;
            }

            case JavaInstruction::BiPush:
                frame.stack.push({.valueInt = code->code[pc + 1]});
                break;
            case JavaInstruction::SiPush:
                frame.stack.push({.valueInt = (code->code[pc + 1] << 8) | code->code[pc + 2]});
                break;

            case JavaInstruction::IConst_0:
                frame.stack.push({.valueInt = 0});
                break;
            case JavaInstruction::IConst_1:
                frame.stack.push({.valueInt = 1});
                break;
            case JavaInstruction::IConst_2:
                frame.stack.push({.valueInt = 2});
                break;
            case JavaInstruction::IConst_3:
                frame.stack.push({.valueInt = 3});
                break;
            case JavaInstruction::IConst_4:
                frame.stack.push({.valueInt = 4});
                break;
            case JavaInstruction::IConst_5:
                frame.stack.push({.valueInt = 5});
                break;
            case JavaInstruction::IConst_m1:
                frame.stack.push({.valueInt = -1});
                break;

            case JavaInstruction::AStore_0:
            case JavaInstruction::IStore_0:
                frame.locals[0] = frame.stack.popValue();
                break;
            case JavaInstruction::AStore_1:
            case JavaInstruction::IStore_1:
                frame.locals[1] = frame.stack.popValue();
                break;
            case JavaInstruction::AStore_2:
            case JavaInstruction::IStore_2:
                frame.locals[2] = frame.stack.popValue();
                break;
            case JavaInstruction::AStore_3:
            case JavaInstruction::IStore_3:
                frame.locals[3] = frame.stack.popValue();
                break;

            case JavaInstruction::AStore:
            case JavaInstruction::IStore:
                frame.locals[code->code[pc + 1]] = frame.stack.popValue();
                break;

            case JavaInstruction::Iadd:
                frame.stack.push({.valueInt = frame.stack.popValue().valueInt + frame.stack.popValue().valueInt});
                break;
            case JavaInstruction::Isub: {
                JavaInt value = frame.stack.popValue().valueInt;
                frame.stack.push({.valueInt = frame.stack.popValue().valueInt - value});
                break;
            }
            case JavaInstruction::Imul:
                frame.stack.push({.valueInt = frame.stack.popValue().valueInt * frame.stack.popValue().valueInt});
                break;
            case JavaInstruction::Idiv: {
                JavaInt value = frame.stack.popValue().valueInt;
                frame.stack.push({.valueInt = frame.stack.popValue().valueInt / value});
                break;
            }
            case JavaInstruction::Iinc:
                frame.locals[code->code[pc + 1]].valueInt += code->code[pc + 2];
                break;

            case JavaInstruction::ALoad_0:
            case JavaInstruction::ILoad_0:
                frame.stack.push(frame.locals[0]);
                break;
            case JavaInstruction::ALoad_1:
            case JavaInstruction::ILoad_1:
                frame.stack.push(frame.locals[1]);
                break;
            case JavaInstruction::ALoad_2:
            case JavaInstruction::ILoad_2:
                frame.stack.push(frame.locals[2]);
                break;
            case JavaInstruction::ALoad_3:
            case JavaInstruction::ILoad_3:
                frame.stack.push(frame.locals[3]);
                break;

            case JavaInstruction::ALoad:
            case JavaInstruction::ILoad:
                frame.stack.push(frame.locals[code->code[pc + 1]]);
                break;


            case JavaInstruction::IaLoad: {
                JavaInt index = frame.stack.popValue().valueInt;
                JavaEnvironmentArray *array = (JavaEnvironmentArray *)frame.stack.popValue().valueRef;
                frame.stack.push(array->arrayData[index]);
                break;
            }

            case JavaInstruction::IaStore: {
                JavaTypeAny value = frame.stack.popValue();
                JavaInt index = frame.stack.popValue().valueInt;
                JavaEnvironmentArray *array = (JavaEnvironmentArray *)frame.stack.popValue().valueRef;
                array->arrayData[index] = value;
                break;
            }

            case JavaInstruction::Pop:
                frame.stack.pop();
                break;
            case JavaInstruction::Dup:
                frame.stack.duplicate();
                break;

            case JavaInstruction::InvokeVirtual:
            case JavaInstruction::InvokeSpecial: // TODO: These two need to be implemented properly.
            case JavaInstruction::InvokeStatic: {
                PoolIndex index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                JavaConstantMethodRef *methodRef = JavaConstantMethodRef::cast(method->parent.pool[index]);
                JavaIdentifier envId = JavaIdentifier(methodRef->getNameAndType());
                JavaEnvironmentMachineClass *standardClass = getStandardClass(methodRef->getClass()->getName());
                if (standardClass) {
                    JavaEnvironmentMachineContext context = {*this, frame};
                    standardClass->getMethod(envId).function(context);
                } else {
                    JavaEnvironmentClass *envClass = getClass(methodRef->getClass()->getName());
                    if (!envClass)
                        throw Utils::GenericException("MissingClass",
                            "Cannot find class " + methodRef->getClass()->getName());
                    JavaReturnType returnValue = run(envClass->getMethod(envId), frame,
                        instruction == JavaInstruction::InvokeVirtual
                        || instruction == JavaInstruction::InvokeSpecial);
                    if (returnValue.type != JavaType::Void) frame.stack.push(returnValue.value);
                }
                break;
            }
            case JavaInstruction::Goto: {
                short offset = (code->code[pc + 1] << 8) | code->code[pc + 2];
                pc += offset;
                pc -= info.length;
                break;
            }

            case JavaInstruction::Ifeq:
                if (frame.stack.popValue().valueInt == 0)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::Ifne:
                if (frame.stack.popValue().valueInt != 0)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::Ifle:
                if (frame.stack.popValue().valueInt <= 0)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::Iflt:
                if (frame.stack.popValue().valueInt < 0)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::Ifge:
                if (frame.stack.popValue().valueInt >= 0)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::Ifgt:
                if (frame.stack.popValue().valueInt > 0)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;

            case JavaInstruction::If_icmpeq:
                if (frame.stack.popValue().valueInt == frame.stack.popValue().valueInt)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::If_icmpne:
                if (frame.stack.popValue().valueInt != frame.stack.popValue().valueInt)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::If_icmple:
                if (frame.stack.popValue().valueInt >= frame.stack.popValue().valueInt)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::If_icmplt:
                if (frame.stack.popValue().valueInt > frame.stack.popValue().valueInt)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            case JavaInstruction::If_icmpge: {
                JavaInt value1 = frame.stack.popValue().valueInt, value2 = frame.stack.popValue().valueInt;
                if (value1 <= value2)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;
            }
            case JavaInstruction::If_icmpgt:
                if (frame.stack.popValue().valueInt < frame.stack.popValue().valueInt)
                    pc += ((code->code[pc + 1] << 8) | code->code[pc + 2]) - info.length;
                break;

            case JavaInstruction::IReturn:
                return {JavaType::Int, frame.stack.popValue()};
            case JavaInstruction::Return:
                return {JavaType::Void};

            case JavaInstruction::GetField: {
                PoolIndex index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                JavaConstantFieldRef *fieldRef = JavaConstantFieldRef::cast(method->parent.pool[index]);
                JavaEnvironmentInstance *instance = (JavaEnvironmentInstance *) frame.stack.popValue().valueRef;
                frame.stack.push(instance->fields[fieldRef->getNameAndType()->getName()]);
                break;
            }
            case JavaInstruction::PutField: {
                PoolIndex index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                JavaConstantFieldRef *fieldRef = JavaConstantFieldRef::cast(method->parent.pool[index]);
                JavaTypeAny value = frame.stack.popValue();
                JavaEnvironmentInstance *instance = (JavaEnvironmentInstance *) frame.stack.popValue().valueRef;
                instance->fields[fieldRef->getNameAndType()->getName()] = value;
                break;
            }
            case JavaInstruction::GetStatic: {
                PoolIndex index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                JavaConstantFieldRef *fieldRef = JavaConstantFieldRef::cast(method->parent.pool[index]);
                JavaEnvironmentMachineClass *standardClass = getStandardClass(fieldRef->getClass()->getName());
                if (standardClass) {
                    frame.stack.push(
                        standardClass->getField(
                            JavaIdentifier(fieldRef->getNameAndType())).value);
                } else {
                    JavaEnvironmentClass *loadedClass = getClass(fieldRef->getClass()->getName());
                    if (!loadedClass)
                        throw Utils::GenericException("MissingClass",
                            "Cannot find class " + fieldRef->getClass()->getName());

                    throw std::exception();
                }
                break;
            }

            case JavaInstruction::New: {
                PoolIndex index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                JavaConstantClass *type = JavaConstantClass::cast(method->parent.pool[index]);
                JavaEnvironmentMachineClass *standardClass = getStandardClass(type->getName());
                if (standardClass) {
                    JavaEnvironmentInstance *instance = new JavaEnvironmentInstance(*standardClass);
                    frame.stack.push({.valueRef = instance});
                } else {
                    JavaEnvironmentClass *loadedClass = getClass(type->getName());
                    if (!loadedClass)
                        throw Utils::GenericException("MissingClass", "Cannot find class " + type->getName());

                    JavaEnvironmentInstance *instance = new JavaEnvironmentInstance(*loadedClass, false);
                    frame.stack.push({.valueRef = instance});
                }
                break;
            }
            case JavaInstruction::NewArray: {
                JavaInt size = frame.stack.popValue().valueInt;
                JavaArrayType type = (JavaArrayType)code->code[pc + 1];
                frame.stack.push({ .valueRef = new JavaEnvironmentArray(type, size) });
                break;
            }

            case JavaInstruction::ArrayLength: {
                JavaEnvironmentArray *array = (JavaEnvironmentArray *)frame.stack.popValue().valueRef;
                frame.stack.push({ .valueInt = array->size });
                break;
            }

            default:
                Utils::log("Unimplemented instruction\n"
                + createDisasm(method->parent, &code->code[pc], info.length, pc));
                break;
        }

        pc += info.length;
    }

    return {JavaType::Void};
}

JavaReturnType JavaEnvironment::run(const JavaMethod &method, JavaEnvironmentFrame &lastFrame, bool reference) {
    JavaAttributeCode *code = method->getCode();
    JavaEnvironmentFrame frame = JavaEnvironmentFrame(code->maxLocals, code->maxStack,
                                                      lastFrame, method->getDescriptor(), reference);

    return execute(method, code, frame);
}

void JavaEnvironment::run(const JavaMethod &method) {
    JavaAttributeCode *code = method->getCode();
    JavaEnvironmentFrame frame = JavaEnvironmentFrame(code->maxLocals, code->maxStack);

    execute(method, code, frame);
}

void JavaEnvironment::run() {
    run(getClass(mainClass)->getMethod(mainId));
}

JavaEnvironmentClass &JavaEnvironment::loadClass(const JavaClass &java) {
    classes.emplace_back(java);
    return classes[classes.size() - 1];
}

JavaEnvironment::JavaEnvironment(const JavaClass &java)
    : mainClass(java.getThisClass()->getName()), standard(getStandard()) {
    loadClass(java);
}

JavaEnvironment::JavaEnvironment(const JavaJar &jar) : JavaEnvironment(*jar[jar.mainIndex]) {
    for (std::shared_ptr<JavaClass> java : jar) {
        if (mainClass != java->getThisClass()->getName()) {
            loadClass(*java);
        }
    }
}
