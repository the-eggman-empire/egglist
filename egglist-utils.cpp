#include "egglist-utils.h"

WriteMode EggListUtils::writeMode = Console;
std::fstream EggListUtils::outFile;

void EggListUtils::setWriteMode(WriteMode writeMode) {
    EggListUtils::writeMode = writeMode;
}

void EggListUtils::setOutFile(std::string fileName) {
    writeMode = File;
    outFile.open(fileName, std::ios::out);
}

void EggListUtils::closeOutFile() {
    outFile.close();
}

void EggListUtils::write(std::string word) {
    if (writeMode == File) {
        outFile << word << std::endl;
    } else if (writeMode == Console) {
        std::cout << word << std::endl;
    }
}