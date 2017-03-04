#include "egglist-engine.h"


EggListEngine *EggListEngine::instance = NULL;
std::mutex EggListEngine::m;
ulong EggListEngine::totalSize;
ulong EggListEngine::fileSize;

EggListEngine::EggListEngine() {
    config = new EggListConfig();
    totalSize = 0;
    fileSize = 0;
}

EggListEngine::EggListEngine(EggListConfig *config) {
    this->config = config;
    totalSize = 0;
    fileSize = 0;
}

EggListEngine::EggListEngine(int argc, char *argv[]) {
    config = new EggListConfig(argc, argv);
    totalSize = 0;
    fileSize = 0;
}

EggListEngine *EggListEngine::engine() {
    if (!instance)
        instance = new EggListEngine();

    return instance;
}

EggListEngine *EggListEngine::engine(EggListConfig *config) {
    if (!instance)
        instance = new EggListEngine(config);

    return instance;
}

EggListEngine *EggListEngine::engine(int argc, char *argv[]) {
    if (!instance)
        instance = new EggListEngine(argc, argv);

    return instance;
}

void EggListEngine::shutdownHandler(int signal) {
    instance->stop();
}

void EggListEngine::start() {
    shutdownFlag = false;
    config->setStartTime(std::chrono::high_resolution_clock::now());

    /*struct sigaction sigint_handler;
    sigint_handler.sa_handler = shutdownHandler;
    sigemptyset(&sigint_handler.sa_mask);
    sigint_handler.sa_flags = 0;
    sigaction(SIGINT, &sigint_handler, NULL);*/
    signal(SIGINT, EggListEngine::shutdownHandler);

    switch (config->getMode()) {
        case Random:
            startRandomEngine();
            break;
        case Hex:
            startHexEngine();
            break;
        case Dictionary:
            startDictionaryEngine();
            break;
        default:
            if (config->isResume()) {
                resumeEngine();
            } else {
                startEngine();
            }
            break;
    }

    config->setStopTime(std::chrono::high_resolution_clock::now());
    config->writeSession();

    std::cout << "--" << std::endl;
    std::cout << "egglist execution stopped" << std::endl;
    std::cout << "total runtime: " << config->getRunTime() << "ms" << std::endl;
    std::cout << "total iterations: " << config->getIterations() << std::endl;
    std::cout << "average speed: " << config->getIterationsPerSecond() << " i/s" << std::endl;
}

void EggListEngine::stop() {
    shutdownFlag = true;
}

void EggListEngine::startEngine() {

    for (uint l = config->getMinLength(); l <= config->getMaxLength(); ++l) {
        std::string w(l, '\0');
        generate(0, l, w, false);
    }
}

void EggListEngine::resumeEngine() {
    word = config->getResumeWord();
    uint start_len = (uint) word.length();
    for (uint l = start_len; l <= config->getMaxLength(); ++l) {
        generate(0, l, word, (l == start_len));
    }
}

void EggListEngine::generate(uint pos, uint len, std::string word, bool resume) {

    if (shutdownFlag) {
        return;
    }

    if (pos < len) {

        unsigned short start_index = (resume && config->getCharList().find_first_of(word[pos] != std::string::npos)) ?
                                     (unsigned short)config->getCharList().find_first_of(word[pos]) :
                                     (unsigned short)0;

        for (unsigned short i = start_index; i < config->getCharList().length(); ++i) {
            word[pos] = config->getCharList()[i];
            generate(pos + 1, len, word, resume);
            resume = false;
        }

        return;
    }

    config->setResumeWord(word);
    config->incrementIterations();
    std::cout << word << std::endl;
    totalSize += word.length();
    fileSize += word.length();
}

void EggListEngine::startRandomEngine() {
    threads = new std::thread[config->getThreadCount()];

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i] = std::thread(generateRandom);
    }

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i].join();
    }
}

void EggListEngine::generateRandom() {
    std::lock_guard<std::mutex> lock(m);
    std::srand((uint)std::time(0));
    uint range = (instance->config->getMaxLength() - instance->config->getMinLength()) + 1;
    uint len;
    std::string word;
    while(!instance->shutdownFlag) {
        len = std::rand() % range + instance->config->getMinLength();
        word = std::string(len, '\0');
        std::generate_n(word.begin(), len, getRandomChar);
        std::cout << word << std::endl;
        totalSize += word.length();
        fileSize += word.length();
        instance->config->incrementIterations();
    }
}

char EggListEngine::getRandomChar() {
    return instance->config->getCharList()[std::rand() % (instance->config->getCharList().length() - 1)];
}

void EggListEngine::startHexEngine() {

    threads = new std::thread[config->getThreadCount()];

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i] = std::thread(generateRandomHex);
    }

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i].join();
    }
}

void EggListEngine::generateRandomHex() {
    std::lock_guard<std::mutex> lock(m);
    std::srand((uint)std::time(0));
    uint range = (instance->config->getMaxLength() - instance->config->getMinLength()) + 1;
    ushort hexRange = (255 - 0) + 1;
    uint len;

    while(!instance->shutdownFlag) {
        len = std::rand() % range + instance->config->getMinLength();
        for (ushort i = 0; i < len; ++i)
            std::cout << std::hex << (ushort)(std::rand() % hexRange + 0);
        std::cout << std::endl;

        totalSize += (len * 2);
        fileSize += (len * 2);
        instance->config->incrementIterations();
    }
}

void EggListEngine::startDictionaryEngine() {
    config->readDictionary();
    std::string w = "";
    uint maxWords = (uint)(config->getMaxLength() / config->getDictionary()[0].length());
    generateDictionaryWord(0, maxWords, w);
}

void EggListEngine::generateDictionaryWord(uint pos, uint len, std::string word) {
    if (shutdownFlag) {
        return;
    }

    if (pos < len) {

        std::string w = word;

        for (uint i = 0; i < config->getDictionary().size(); ++i) {
            if (shutdownFlag)
                break;

            w += config->getDictionary()[i];
            if (w.length() > config->getMaxLength())
                break;

            std::cout << w << std::endl;

            totalSize += w.length();
            fileSize += w.length();

            config->incrementIterations();
            config->setResumeWord(word);

            generateDictionaryWord(pos + 1, len, w);



            w = word;
        }
    }
}