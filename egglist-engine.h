#ifndef EGGLIST_ENGINE_H
#define EGGLIST_ENGINE_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <signal.h>

#include "egglist-config.h"


class EggListEngine {
private:
    static std::mutex m;
    std::string word;
    EggListConfig *config;
    bool shutdownFlag;
    std::thread *threads;

    static ulong totalSize;
    static ulong fileSize;

    static EggListEngine *instance;

    static void shutdownHandler(int signal);

    EggListEngine();
    EggListEngine(EggListConfig *config);
    EggListEngine(int argc, char *argv[]);

protected:
    void startEngine();
    void resumeEngine();
    void generate(uint pos, uint len, std::string word, bool resume);

    void startRandomEngine();
    static void generateRandom();
    static char getRandomChar();

    void startHexEngine();
    static void generateRandomHex();

    void startDictionaryEngine();
    void generateDictionaryWord(uint pos, uint len, std::string word);

public:

    static EggListEngine *engine();
    static EggListEngine *engine(EggListConfig *config);
    static EggListEngine *engine(int argc, char *argv[]);

    void start();
    void stop();
};


#endif //EGGLIST_ENGINE_H
