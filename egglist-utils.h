#ifndef EGGLIST_UTILS_H
#define EGGLIST_UTILS_H

#include <iostream>
#include <fstream>
#include <string>

#include "egglist-defaults.h"

enum WriteMode {
    File, Console
};

class EggListUtils {
private:
    static WriteMode writeMode;
    static std::fstream outFile;

public:
    static void setOutFile(std::string fileName);
    static void closeOutFile();
    static void setWriteMode(WriteMode writeMode);
    static void write(std::string word);
};


#endif //EGGLIST_UTILS_H
