#include "engine.h"


Engine *Engine::instance = NULL;
std::mutex Engine::m;
ulong Engine::totalSize;
ulong Engine::fileSize;

Engine::Engine() {
    config = new Config();
    totalSize = 0;
    fileSize = 0;
}

Engine::Engine(Config *config) {
    this->config = config;
    totalSize = 0;
    fileSize = 0;
}

Engine::Engine(int argc, char *argv[]) {
    config = new Config(argc, argv);
    totalSize = 0;
    fileSize = 0;
}

Engine *Engine::engine() {
    if (!instance)
        instance = new Engine();

    return instance;
}

Engine *Engine::engine(Config *config) {
    if (!instance)
        instance = new Engine(config);

    return instance;
}

Engine *Engine::engine(int argc, char *argv[]) {
    if (!instance)
        instance = new Engine(argc, argv);

    return instance;
}

void Engine::shutdownHandler(int signal) {
    instance->stop();
}

void Engine::start() {
    shutdownFlag = false;
    config->setStartTime(std::chrono::high_resolution_clock::now());

    signal(SIGINT, Engine::shutdownHandler);

    if (Utils::getWriteMode() == File)
        config->initNextOutFile();

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

    Utils::closeOutFile();
    config->setStopTime(std::chrono::high_resolution_clock::now());
    config->writeSession();

    std::cout << "--" << std::endl;
    std::cout << "egglist execution stopped" << std::endl;
    std::cout << "total runtime: " << config->getRunTime() << "ms" << std::endl;
    std::cout << "total iterations: " << config->getIterations() << std::endl;
    std::cout << "average speed: " << config->getIterationsPerSecond() << " i/s" << std::endl;
}

void Engine::stop() {
    shutdownFlag = true;
}

void Engine::startEngine() {

    for (uint l = config->getMinLength(); l <= config->getMaxLength(); ++l) {
        std::string w(l, '\0');
        generate(0, l, w, false);
    }
}

void Engine::resumeEngine() {
    word = config->getResumeWord();
    uint start_len = (uint) word.length();
    for (uint l = start_len; l <= config->getMaxLength(); ++l) {
        generate(0, l, word, (l == start_len));
    }
}

void Engine::generate(uint pos, uint len, std::string word, bool resume) {

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

    Utils::write(word);
    updateSizes(word.length());
}

void Engine::startRandomEngine() {
    threads = new std::thread[config->getThreadCount()];

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i] = std::thread(generateRandom);
    }

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i].join();
    }
}

void Engine::generateRandom() {
    std::lock_guard<std::mutex> lock(m);
    std::srand((uint)std::time(0));
    uint range = (instance->config->getMaxLength() - instance->config->getMinLength()) + 1;
    uint len;
    std::string word;
    while(!instance->shutdownFlag) {
        len = std::rand() % range + instance->config->getMinLength();
        word = std::string(len, '\0');
        std::generate_n(word.begin(), len, getRandomChar);

        Utils::write(word);

        updateSizes(word.length());
        instance->config->incrementIterations();
    }
}

char Engine::getRandomChar() {
    return instance->config->getCharList()[std::rand() % (instance->config->getCharList().length() - 1)];
}

void Engine::startHexEngine() {

    threads = new std::thread[config->getThreadCount()];

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i] = std::thread(generateRandomHex);
    }

    for (ushort i = 0; i < config->getThreadCount(); ++i) {
        threads[i].join();
    }
}

void Engine::generateRandomHex() {
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

        updateSizes(len * 2);
        instance->config->incrementIterations();
    }
}

void Engine::startDictionaryEngine() {
    config->readDictionary();
    std::string w = "";
    uint maxWords = (uint)(config->getMaxLength() / config->getDictionary()[0].length());
    generateDictionaryWord(0, maxWords, w);
}

void Engine::generateDictionaryWord(uint pos, uint len, std::string word) {
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

            Utils::write(w);

            updateSizes(w.length());

            config->incrementIterations();
            config->setResumeWord(word);

            generateDictionaryWord(pos + 1, len, w);



            w = word;
        }
    }
}

void Engine::updateSizes(ulong length) {
    if (length == 0) {
        totalSize = 0;
        fileSize = 0;
        return;
    }

    totalSize += length;
    fileSize += length;

    if (totalSize >= instance->config->getMaxSize()) {
        instance->shutdownFlag = true;
        Utils::closeOutFile();
    }

    if (fileSize >= instance->config->getMaxFileSize()) {
        instance->config->initNextOutFile();
        fileSize = 0;
    }
}