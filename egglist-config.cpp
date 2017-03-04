#include "egglist-config.h"

EggListConfig::EggListConfig() :
        specialChars("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"),
        numericChars("0123456789"),
        alphaLowerChars("abcdefghijklmnopqrstuvwxyz"),
        alphaUpperChars("ABCDEFGHIJKLMNOPQRSTUVWXYZ") {

    std::srand((uint)std::time(0));

    sessionName = DEFAULT_SESSION_NAME;
    minLength = DEFAULT_MIN_LENGTH;
    maxLength = DEFAULT_MAX_LENGTH;
    charList = "";
    resumeWord = "";
    threadCount = DEFAULT_THREAD_COUNT;
    iterations = 0;
    inFile = DEFAULT_DICTIONARY_FILE;
}

EggListConfig::EggListConfig(int argc, char* argv[]) : EggListConfig::EggListConfig() {
    parseCommandLine(argc, argv);
}

EggListConfig::~EggListConfig() {

}

void EggListConfig::parseCommandLine(int argc, char *argv[]) {
    int opt;
    bool shuffle = false;
    bool dedupe = false;
    while ((opt = getopt(argc, argv, "he:l:c:r:n:m:t:m:i:o:sd")) != EOF) {
        switch (opt) {
            case 'h':
                printHelp();
                break;
            case 'e': {
                std::string flags = optarg;
                for (uint i = 0; i < flags.length(); ++i) {
                    switch (flags[i]) {
                        case 'a':
                            enableAlphaLowerChars();
                            break;
                        case 'A':
                            enableAlphaUpperChars();
                            break;
                        case '1':
                            enableNumericChars();
                            break;
                        case '@':
                            enableSpeciaChars();
                            break;
                        case 's':
                            enableSpaceChar();
                            break;
                    }
                }
                break;
            }
            case 's':
                if (resumeWord.length() == 0)
                    shuffle = true;
                break;
            case 'd':
                dedupe = true;
                break;
            case 'l': {
                std::string length_str = optarg;
                if (length_str.find_first_of("-") != std::string::npos) {
                    std::string min_str = length_str.substr(0, length_str.find("-"));
                    std::string max_str = length_str.substr(length_str.find("-") + 1, length_str.length());
                    std::istringstream (min_str) >> minLength;
                    std::istringstream (max_str) >> maxLength;
                } else {
                    std::istringstream(length_str) >> minLength;
                    std::istringstream(length_str) >> maxLength;
                }
                break;
            }
            case 't': {
                std::istringstream (optarg) >> threadCount;
                break;
            }
            case 'c': {
                std::ifstream f(optarg);
                if (f.good()) {
                    std::stringstream buffer;
                    buffer << f.rdbuf();
                    appendCharList(buffer.str());
                    f.close();
                } else {
                    appendCharList(optarg);
                }
                break;
            }
            case 'r': {
                std::string s = optarg;
                if (s.compare(".") == 0)
                    s = DEFAULT_SESSION_NAME;

                std::ifstream f(s + SESSION_EXTENSION);
                if (f.good()) {
                    f.close();
                    sessionName = s;
                    readSession();
                } else {
                    setResumeWord(optarg);
                }
                break;
            }
            case 'n': {
                sessionName = optarg;
                break;
            }
            case 'm': {
                std::string m = optarg;
                switch (m[0]) {
                    case 'r':
                        mode = Random;
                        break;
                    case 'h':
                        mode = Hex;
                        break;
                    case 'd':
                        mode = Dictionary;
                        break;
                    default:
                        mode = Permutation;
                        break;
                }
                break;
            }
            case 'i': {
                std::ifstream is(optarg);
                if (!is.good()) {
                    std::cerr << "Could not find in-file " << optarg << std::endl;
                    break;
                }
                is.close();
                inFile = optarg;
                break;
            }
            case 'o': {
                outFile = optarg;
                break;
            }
        }
    }

    if (charList.length() == 0) {
        enableDefaultCharList();
    } else {
        charList.erase(std::remove(charList.begin(), charList.end(), '\n'), charList.end());
    }

    if (dedupe) {
        std::sort(charList.begin(), charList.end());
        charList.erase(std::unique(charList.begin(), charList.end()), charList.end());
    }

    if (shuffle) {
        std::random_shuffle(charList.begin(), charList.end());
    }
}

void EggListConfig::printHelp() {
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "  ______            _ _     _   " << std::endl;
    std::cout << " |  ____|          | (_)   | |  " << std::endl;
    std::cout << " | |__   __ _  __ _| |_ ___| |_ " << std::endl;
    std::cout << " |  __| / _` |/ _` | | / __| __|" << std::endl;
    std::cout << " | |___| (_| | (_| | | \\__ \\ |_ " << std::endl;
    std::cout << " |______\\__, |\\__, |_|_|___/\\__|" << std::endl;
    std::cout << "         __/ | __/ |            " << std::endl;
    std::cout << "        |___/ |___/             " << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "An official product of The Eggman Empire" << std::endl;
    std::cout << "Author: Dr. Eggman" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "-m r | h | d | p: specify engine mode (default: p)" << std::endl;
    std::cout << "    r: random word (threaded)" << std::endl;
    std::cout << "    h: random hex (threaded)" << std::endl;
    std::cout << "    d: dictionary" << std::endl;
    std::cout << "    p: permutation (resumeable)" << std::endl;
    std::cout << std::endl;

    std::cout << "-e aA1@s: enable features (default: aA1@s)" << std::endl;
    std::cout << "    a: a-z (permutation and random-word modes)" << std::endl;
    std::cout << "    A: A-Z (permutation and random-word modes)" << std::endl;
    std::cout << "    1: 0-9 (permutation and random-word modes)" << std::endl;
    std::cout << "    @: symbols (permutation and random-word modes)" << std::endl;
    std::cout << "    s: space (permutation and random-word modes)" << std::endl;
    std::cout << std::endl;

    std::cout << "-s: shuffle characters (permutation and random-word modes)" << std::endl;
    std::cout << std::endl;

    std::cout << "-d: remove duplicate characters (permutation and random-word modes)" << std::endl;
    std::cout << std::endl;

    std::cout << "-l <length> | <min>-<max>: length of words generated (default: 8-63)" << std::endl;
    std::cout << "    <length>: fixed length of word" << std::endl;
    std::cout << "    <min>-<max>: length range" << std::endl;
    std::cout << std::endl;

    std::cout << "-t <count>: number of threads (default: 2) (random word and hex modes)" << std::endl;
    std::cout << std::endl;

    std::cout << "-c <chars> | <file>: custom charactes" << std::endl;
    std::cout << "    <chars>: a list of characters to use" << std::endl;
    std::cout << "    <file>: a filename containing characters to use (not a dictionary)" << std::endl;
    std::cout << std::endl;

    std::cout << "-r . | <session>: resume a session" << std::endl;
    std::cout << "    .: resume default session in 'session.egglist'" << std::endl;
    std::cout << std::endl;

    std::cout << "-n <session>: specify session name" << std::endl;
    std::cout << std::endl;

    std::cout << "-i <file>: wordlist for dictionary mode" << std::endl;
    std::cout << std::endl;

    exit(0);
}

uint EggListConfig::getMinLength() {
    return minLength;
}

void EggListConfig::setMinLength(uint minLength) {
    this->minLength = minLength;
}

uint EggListConfig::getMaxLength() {
    return maxLength;
}

void EggListConfig::setMaxLength(uint maxLength) {
    this->maxLength = maxLength;
}

EngineMode EggListConfig::getMode() {
    return mode;
}

void EggListConfig::setMode(EngineMode mode) {
    this->mode = mode;
}

std::chrono::high_resolution_clock::time_point EggListConfig::getStartTime() {
    return startTime;
}
void EggListConfig::setStartTime(std::chrono::high_resolution_clock::time_point startTime) {
    this->startTime = startTime;
}

std::chrono::high_resolution_clock::time_point EggListConfig::getStopTime() {
    return stopTime;
}

void EggListConfig::setStopTime(std::chrono::high_resolution_clock::time_point stopTime) {
    this->stopTime = stopTime;
}

double EggListConfig::getRunTime() {
    std::chrono::duration<double, std::milli> span = stopTime - startTime;
    return span.count();
}

uint EggListConfig::getIterationsPerSecond() {
    ulong seconds = (ulong)getRunTime() / 1000;
    return (uint)(iterations / seconds);
}

std::string EggListConfig::getSpecialChars() {
    return specialChars;
}

std::string EggListConfig::getNumericChars() {
    return numericChars;
}

std::string EggListConfig::getAlphaLowerChars() {
    return alphaLowerChars;
}

std::string EggListConfig::getAlphaUpperChars() {
    return alphaUpperChars;
}

std::vector<std::string> &EggListConfig::getDictionary() {
    return dictionary;
}

void EggListConfig::readDictionary() {
    std::ifstream is(inFile);
    if (!is.good()) {
        std::cerr << "Failed to read dictionary file " << inFile << std::endl;
        exit(-1);
    }
    std::istream_iterator<std::string> start(is), end;
    dictionary = std::vector<std::string>(start, end);

    StringCompare sc;
    std::sort(dictionary.begin(), dictionary.end(), sc);

    StringFilter filter(3, maxLength);
    std::vector<std::string> out;
    std::remove_copy_if(dictionary.begin(), dictionary.end(), std::back_inserter(out), std::not1(filter));
    dictionary = out;
}

std::string EggListConfig::getCharList() {
    return charList;
}

void EggListConfig::setCharList(std::string charList) {
    this->charList = charList;
}

void EggListConfig::appendCharList(std::string charList) {
    this->charList += charList;
}

void EggListConfig::enableDefaultCharList() {
    charList = specialChars + numericChars + alphaLowerChars + alphaUpperChars + ' ';
}

void EggListConfig::enableSpeciaChars() {
    charList += specialChars;
}

void EggListConfig::enableNumericChars() {
    charList += numericChars;
}

void EggListConfig::enableAlphaLowerChars() {
    charList += alphaLowerChars;
}

void EggListConfig::enableAlphaUpperChars() {
    charList += alphaUpperChars;
}

void EggListConfig::enableSpaceChar() {
    charList += ' ';
}

std::string EggListConfig::getResumeWord() {
    return resumeWord;
}

void EggListConfig::setResumeWord(std::string resumeWord) {
    this->resumeWord = resumeWord;
}

bool EggListConfig::isResume() {
    return resumeWord.length() > 0;
}

ushort EggListConfig::getThreadCount() {
    return threadCount;
}

void EggListConfig::setThreadCount(ushort threadCount) {
    this->threadCount = threadCount;
}

ulong EggListConfig::getIterations() {
    return iterations;
}

void EggListConfig::incrementIterations() {
    ++iterations;
}

void EggListConfig::writeSession() {
    std::ofstream o(sessionName + ".egglist");
    json j = {
            {"minLength", minLength},
            {"maxLength", maxLength},
            {"charList", charList},
            {"resumeWord", resumeWord},
            {"mode", mode},
            {"threadCount", threadCount},
            {"inFile", inFile}
    };
    o << std::setw(4) << j << std::endl;
}

void EggListConfig::readSession() {
    std::ifstream i(sessionName + ".egglist");
    json j;
    i >> j;

    minLength = j["minLength"];
    maxLength = j["maxLength"];
    charList = j["charList"];
    resumeWord = j["resumeWord"];
    mode = j["mode"];
    threadCount = j["threadCount"];
    inFile = j["inFile"];
}