#ifndef KONA_JAVAJAR_H
#define KONA_JAVAJAR_H

#include <Utils.h>

#include <Kophi/JavaClass.h>

using namespace Kophi;

class JavaJar : public std::vector<std::shared_ptr<JavaClass>> {
public:
    int mainIndex = -1;

    explicit JavaJar(const std::string &path);
};

#endif //KONA_JAVAJAR_H
