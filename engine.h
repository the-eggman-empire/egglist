#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <signal.h>

#include "config.h"
#include "utils.h"

class Engine {
private:
    static std::mutex m;
    std::string word;
    Config *config;
    bool shutdownFlag;
    std::thread *threads;

    static ulong totalSize;
    static ulong fileSize;

    static Engine *instance;

    static void shutdownHandler(int signal);

    Engine();

    Engine(Config *config);

    Engine(int argc, char *argv[]);

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

    static void updateSizes(ulong length);

public:

    static Engine *engine();

    static Engine *engine(Config *config);

    static Engine *engine(int argc, char *argv[]);

    void start();
    void stop();
};


#endif //ENGINE_H
