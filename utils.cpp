#include "utils.h"

WriteMode Utils::writeMode = Console;
std::fstream Utils::outFile;

void Utils::setWriteMode(WriteMode writeMode) {
    Utils::writeMode = writeMode;
}

WriteMode Utils::getWriteMode() {
    return writeMode;
}

void Utils::openOutFile(std::string fileName, bool resume) {
    writeMode = File;
    outFile.open(fileName, std::ios::out);
}

void Utils::openOutFile(std::string fileName) {
    std::cout << "Writing to " << fileName << std::endl;
    openOutFile(fileName, false);
}

void Utils::closeOutFile() {
    if (writeMode != File)
        return;

    std::cout << "Closing file" << std::endl;
    outFile.close();
    writeMode = Console;
}

void Utils::write(std::string word) {
    if (writeMode == File) {
        outFile << word << std::endl;
    } else if (writeMode == Console) {
        std::cout << word << std::endl;
    }
}

uint Utils::toInt(std::string s) {
    uint i;
    std::istringstream(s) >> i;
    return i;
}