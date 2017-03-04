#ifndef EGGLIST_CONFIG_H
#define EGGLIST_CONFIG_H

#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
#include <chrono>
#include <ratio>
#include <atomic>
#include <iterator>
#include <vector>
#include <iostream>
#include <unistd.h>
#include "json/json.hpp"

#include "egglist-defaults.h"

using json = nlohmann::json;

enum EngineMode {
    Permutation, Random, Hex, Dictionary
};

class EggListConfig {
private:
    std::string sessionName;
    uint minLength;
    uint maxLength;
    const std::string specialChars;
    const std::string numericChars;
    const std::string alphaLowerChars;
    const std::string alphaUpperChars;
    std::string charList;
    std::string resumeWord;
    ushort threadCount;
    EngineMode mode;
    std::string inFile;
    std::string outFile;
    std::vector<std::string> dictionary;

    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    std::atomic_ulong iterations;

protected:
    void parseCommandLine(int argc, char* argv[]);

public:
    EggListConfig();
    EggListConfig(int argc, char* argv[]);
    ~EggListConfig();

    void printHelp();

    uint getMinLength();
    void setMinLength(uint minLength);

    uint getMaxLength();
    void setMaxLength(uint maxLength);

    EngineMode getMode();
    void setMode(EngineMode mode);

    std::chrono::high_resolution_clock::time_point getStartTime();
    void setStartTime(std::chrono::high_resolution_clock::time_point startTime);

    std::chrono::high_resolution_clock::time_point getStopTime();
    void setStopTime(std::chrono::high_resolution_clock::time_point stopTime);

    double getRunTime();
    uint getIterationsPerSecond();

    std::string getSpecialChars();
    std::string getNumericChars();
    std::string getAlphaLowerChars();
    std::string getAlphaUpperChars();

    std::vector<std::string> &getDictionary();

    std::string getCharList();
    void setCharList(std::string charList);
    void appendCharList(std::string charList);

    void enableDefaultCharList();
    void enableSpeciaChars();
    void enableNumericChars();
    void enableAlphaLowerChars();
    void enableAlphaUpperChars();
    void enableSpaceChar();

    std::string getResumeWord();
    void setResumeWord(std::string resumeWord);
    bool isResume();

    ushort getThreadCount();
    void setThreadCount(ushort threadCount);

    ulong getIterations();
    void incrementIterations();

    void writeSession();
    void readSession();
    void readDictionary();
};


#endif //EGGLIST_CONFIG_H
