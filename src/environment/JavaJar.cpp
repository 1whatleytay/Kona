#include <JavaJar.h>

#include <zip.h>

JavaJar::JavaJar(const std::string &path) {
    zip_t* jar = zip_open(path.c_str(), ZIP_RDONLY, nullptr);
    if (!jar) throw std::exception();

    zip_int64_t fileCount = zip_get_num_entries(jar, 0);
    zip_int64_t metaInf = zip_name_locate(jar, "META-INF/MANIFEST.MF", 0);
    if (metaInf == -1) throw std::exception();

    zip_stat_t metaStat;
    zip_stat_index(jar, metaInf, ZIP_STAT_SIZE, &metaStat);

    std::vector<char> metaData = std::vector<char>(metaStat.size);
    zip_file_t *metaFile = zip_fopen_index(jar, metaInf, 0);
    if (!metaFile) throw std::exception();
    zip_fread(metaFile, &metaData[0], metaStat.size);
    std::string metaText = std::string(metaData.begin(), metaData.end());

    std::string mainClassName;

    std::vector<std::string> params = Utils::split(metaText, '\n');
    for (const std::string &param : params) {
        std::string name = param.substr(0, param.find(':'));
        std::string result = Utils::trim(param.substr(param.find(':') + 1));

        if (name == "Main-Class") {
            mainClassName = result;
        }
    }

    if (mainClassName.empty()) throw std::exception();

    for (zip_int64_t index = 0; index < fileCount; index++) {
        if (index == metaInf) continue;

        zip_stat_t fileStat;
        zip_stat_index(jar, index, ZIP_STAT_SIZE, &fileStat);

        zip_file_t *file = zip_fopen_index(jar, index, 0);
        if (!file) throw std::exception();
        std::vector<Byte> fileData = std::vector<Byte>(fileStat.size);
        zip_fread(file, &fileData[0], fileStat.size);

        if (fileData.empty()) continue;

        emplace_back(new JavaClass(fileData));
        if (operator[](size() - 1)->getThisClass()->getName() == mainClassName) {
            mainIndex = size() - 1;
        }

        zip_fclose(file);
    }
}
