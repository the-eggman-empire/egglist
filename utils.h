#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "defaults.h"

enum WriteMode {
    File, Console
};

class Utils {
private:
    static WriteMode writeMode;
    static std::fstream outFile;

    Utils() {}

public:

    static void setWriteMode(WriteMode writeMode);

    static WriteMode getWriteMode();

    static void openOutFile(std::string fileName, bool resume);

    static void openOutFile(std::string fileName);

    static void closeOutFile();
    static void write(std::string word);

    static uint toInt(std::string s);
};


#endif //UTILS_H
